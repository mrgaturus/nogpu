// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/framebuffer.h"
#include "private/context.h"
#include "private/glad.h"

// -------------------------------
// OpenGL RenderBuffer: Attachment
// -------------------------------

static GPUFrameBufferStatus toValue(GLenum status) {
    switch (status) {
        default:
            return GPUFrameBufferStatus::FRAMEBUFFER_UNSUPPORTED;
        case GL_FRAMEBUFFER_UNDEFINED:
            return GPUFrameBufferStatus::FRAMEBUFFER_UNDEFINED;

        // Incomplete Framebuffer Errors
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            return GPUFrameBufferStatus::FRAMEBUFFER_INCOMPLETE_ATTACHMENT;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            return GPUFrameBufferStatus::FRAMEBUFFER_INCOMPLETE_MULTISAMPLE;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            return GPUFrameBufferStatus::FRAMEBUFFER_INCOMPLETE_MISSING;
    }
}

void GLFrameBuffer::updateAttachment(GLenum attachment, GLRenderLink* link) {
    GLRenderBuffer* target = link->target;
    GLint layer = link->slice.layer;
    GLint level = link->slice.level;

    // Check Attachment Existence
    if (target == nullptr) {
        if (link->tex_cache != 0) {
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                attachment, GL_TEXTURE_2D, 0, 0);
            // Remove Cache
            link->tex_cache = 0;
            return;
        }
    } else if (link->tex_cache == target->m_tex)
        return; // Skip Cached Attachments

    target->updateExternal();
    GLuint tex = target->m_tex;
    link->tex_cache = tex;
    // Update OpenGL Object
    switch (target->m_mode) {
        case GPURenderBufferMode::RENDERBUFFER_UNDEFINED:
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                attachment, GL_TEXTURE_2D, 0, 0);
            GPUReport::warning("attached undefined renderbuffer");
            break;

        // Offscreen Rendering
        case GPURenderBufferMode::RENDERBUFFER_OFFSCREEN:
            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                attachment, GL_RENDERBUFFER, tex);
            break;
        
        // Framebuffer 1D Texture
        case GPURenderBufferMode::RENDERBUFFER_TARGET_1D:
            glFramebufferTexture1D(GL_FRAMEBUFFER,
                attachment, target->m_tex_target, tex, level);
            break;

        // Framebuffer 2D Texture
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_2D:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE_2D:
        case GPURenderBufferMode::RENDERBUFFER_TARGET_2D:
            glFramebufferTexture2D(GL_FRAMEBUFFER,
                attachment, target->m_tex_target, tex, level);
            break;

        // Framebuffer 2D Array, 3D Textures
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_3D:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_ARRAY:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE_ARRAY:
        case GPURenderBufferMode::RENDERBUFFER_TARGET_3D:
        case GPURenderBufferMode::RENDERBUFFER_TARGET_ARRAY:
        case GPURenderBufferMode::RENDERBUFFER_TARGET_CUBEMAP:
        case GPURenderBufferMode::RENDERBUFFER_TARGET_CUBEMAP_ARRAY:
            glFramebufferTextureLayer(GL_FRAMEBUFFER,
                attachment, tex, level, layer);
            break;
    }
}

GPUFrameBufferStatus GLFrameBuffer::checkAttachments() {
    m_ctx->makeCurrentTexture(this);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Check Color Attachments
    GLRenderIndexes* list = &m_colors_index;
    for (int i = 0; i < list->count; i++) {
        GLRenderLink* link = m_colors_index.links[i];
        GLuint index = m_colors_index.indexes[i];
        this->updateAttachment(GL_COLOR_ATTACHMENT0 + index, link);
    }

    // Update Stencil and Depth Attachment
    this->updateAttachment(GL_DEPTH_ATTACHMENT, &m_depth);
    this->updateAttachment(GL_STENCIL_ATTACHMENT, &m_stencil);
    // Check Framebuffer and Return
    GLenum check = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return toValue(check);
}

// -----------------------------------------------
// OpenGL Renderbuffer: Internal Texture Offscreen
// -----------------------------------------------

void GLRenderBuffer::createOffscreen(int w, int h, int samples) {
    m_ctx->makeCurrentTexture(this);
    this->destroyInternal();

    glGenRenderbuffers(1, &m_tex);
    glBindRenderbuffer(GL_RENDERBUFFER, m_tex);
    if (samples <= 1) glRenderbufferStorage(GL_RENDERBUFFER, toValue(m_pixel_type), w, h);
    else glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, toValue(m_pixel_type), w, h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    m_width = w;
    m_height = h;
    m_samples = samples;
    m_tex_target = GL_RENDERBUFFER;
    m_mode = GPURenderBufferMode::RENDERBUFFER_OFFSCREEN;
}

// ----------------------------------------
// OpenGL Renderbuffer: Internal Texture 2D
// ----------------------------------------

void GLRenderBuffer::createTexture2D(int w, int h, int levels, int samples) {
    m_ctx->makeCurrentTexture(this);
    levels = levels_power_of_two(w, h, levels);
    samples = (samples > 0) ? next_power_of_two(samples) : 1;
    // Check Texture Recreate
    this->prepareInternal();

    // Generate New Texture
    glGenTextures(1, &m_tex);
    if (samples <= 1) {
        glBindTexture(GL_TEXTURE_2D, m_tex);
        glTexStorage2D(GL_TEXTURE_2D, levels, toValue(m_pixel_type), w, h);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Set RenderBuffer Modes
        m_target->m_tex_target = GL_TEXTURE_2D;
        m_mode = GPURenderBufferMode::RENDERBUFFER_TEXTURE_2D;
    } else {
        // Check if Multisamples are supported
        if (!GLAD_GL_ARB_texture_storage_multisample) {
            GPUReport::error("multisample textures are not supported");
            destroyInternal();
            return;
        }

        // Create Multisample Texture
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_tex);
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
            samples, toValue(m_pixel_type), w, h, 0);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

        // Set RenderBuffer Modes
        m_target->m_tex_target = GL_TEXTURE_2D_MULTISAMPLE;
        m_mode = GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE_2D;
        // Clamp Mipmap Levels to 1
        if (levels > 1) {
            GPUReport::warning("multisample texture doesn't support mipmap levels");
            levels = 1;
        }
    }

    // Describe RenderBuffer Texture
    m_target->m_levels = levels;
    m_target->m_width = w;
    m_target->m_height = h;
    m_target->m_depth = 1;
    m_samples = samples;
}

void GLRenderBuffer::createTextureArray(int w, int h, int layers, int levels, int samples) {
    m_ctx->makeCurrentTexture(this);
    levels = levels_power_of_two(w, h, levels);
    samples = (samples > 0) ? next_power_of_two(samples) : 1;
    // Check Texture Recreate
    this->prepareInternal();

    // Generate New Texture
    glGenTextures(1, &m_tex);
    if (samples <= 1) {
        glBindTexture(GL_TEXTURE_2D_ARRAY, m_tex);
        glTexStorage3D(GL_TEXTURE_2D_ARRAY,
            levels, toValue(m_pixel_type), w, h, layers);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        // Set RenderBuffer Modes
        m_target->m_tex_target = GL_TEXTURE_2D_ARRAY;
        m_mode = GPURenderBufferMode::RENDERBUFFER_TEXTURE_ARRAY;
    } else {
        // Check if Multisamples are supported
        if (!GLAD_GL_ARB_texture_storage_multisample) {
            GPUReport::error("multisample textures are not supported");
            destroyInternal();
            return;
        }

        // Create Multisample Texture
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, m_tex);
        glTexStorage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
            samples, toValue(m_pixel_type), w, h, layers, 0);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, 0);

        // Set RenderBuffer Modes
        m_target->m_tex_target = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
        m_mode = GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE_ARRAY;
        levels = 1;
    }

    // Describe RenderBuffer Texture
    m_target->m_levels = levels;
    m_target->m_width = w;
    m_target->m_height = h;
    m_target->m_depth = layers;
    m_samples = samples;
}

// ----------------------------------------
// OpenGL Renderbuffer: Internal Texture 3D
// ----------------------------------------

void GLRenderBuffer::createTexture3D(int w, int h, int layers, int levels) {
    m_ctx->makeCurrentTexture(this);
    levels = levels_power_of_two(w, h, levels);
    this->prepareInternal();

    // Generate New Texture
    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_3D, m_tex);
    glTexStorage3D(GL_TEXTURE_3D,
        levels, toValue(m_pixel_type), w, h, layers);
    glBindTexture(GL_TEXTURE_3D, 0);
    // Set RenderBuffer Modes
    m_target->m_tex_target = GL_TEXTURE_3D;
    m_mode = GPURenderBufferMode::RENDERBUFFER_TEXTURE_3D;

    // Describe RenderBuffer Texture
    m_target->m_levels = levels;
    m_target->m_width = w;
    m_target->m_height = h;
    m_target->m_depth = layers;
    m_samples = 1;
}
