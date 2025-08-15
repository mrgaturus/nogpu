// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/framebuffer.h"
#include "private/context.h"
#include "private/glad.h"

GLRenderBuffer::GLRenderBuffer(GLContext* ctx, GPUTexturePixelType type) {
    ctx->gl__makeCurrent();
    if (type == GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED) {
        GPUReport::error("compressed framebuffer texture type is not supported");
        delete this;
        return;
    }

    // Define Renderbuffer initial attributes
    m_mode = GPURenderBufferMode::RENDERBUFFER_UNDEFINED;
    m_pixel_type = type;
    m_ctx = ctx;
}

void GLRenderBuffer::destroyInternal() {
    switch (m_mode) {
        // Destroy Offscreen Render Buffer
        case GPURenderBufferMode::RENDERBUFFER_UNDEFINED: break;
        case GPURenderBufferMode::RENDERBUFFER_OFFSCREEN:
            glDeleteRenderbuffers(1, m_object);
            break;

        // Destroy Texture Render Buffer
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_ARRAY:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE_ARRAY:
            m_target->destroy();
            break;
        
        // Target Cannot be Destroyed
        case GPURenderBufferMode::RENDERBUFFER_TARGET:
        case GPURenderBufferMode::RENDERBUFFER_TARGET_ARRAY:
        case GPURenderBufferMode::RENDERBUFFER_TARGET_CUBEMAP:
        case GPURenderBufferMode::RENDERBUFFER_TARGET_CUBEMAP_ARRAY:
            break;
    }

    // Make RenderBuffer Undefined
    m_mode = GPURenderBufferMode::RENDERBUFFER_UNDEFINED;
    m_target = nullptr;
    m_object = nullptr;
    m_tex = 0;
    m_samples = 0;
    m_width = 0;
    m_height = 0;
}

void GLRenderBuffer::destroy() {
    m_ctx->gl__makeCurrent();

    // Destroy Object
    this->destroyInternal();
    delete this;
}

// ----------------------------
// OpenGL Renderbuffer Checking
// ----------------------------

bool GLRenderBuffer::prepareExternal() {
    bool check = *(m_object) != m_tex;
    if (!check) return check;

    // Check External Mode
    m_tex = *(m_object);
    m_samples = 1;
    switch (m_target->m_tex_target) {
        case GL_TEXTURE_1D:
        case GL_TEXTURE_2D:
        case GL_TEXTURE_RECTANGLE:
            m_mode = GPURenderBufferMode::RENDERBUFFER_TARGET;
            break;

        // Array Textures
        case GL_TEXTURE_3D:
        case GL_TEXTURE_1D_ARRAY:
        case GL_TEXTURE_2D_ARRAY:
            m_mode = GPURenderBufferMode::RENDERBUFFER_TARGET_ARRAY;
            break;

        // Cubemap Textures
        case GL_TEXTURE_CUBE_MAP:
            m_mode = GPURenderBufferMode::RENDERBUFFER_TARGET_CUBEMAP;
            break;
        case GL_TEXTURE_CUBE_MAP_ARRAY_ARB:
            m_mode = GPURenderBufferMode::RENDERBUFFER_TARGET_CUBEMAP_ARRAY;
            break;

        default: // Texture Not Supported
            m_mode = GPURenderBufferMode::RENDERBUFFER_UNDEFINED;
            break;
    }

    // Internal Changed
    return check;
}

bool GLRenderBuffer::prepareInternal() {
    // Check Needs Re-create
    switch (m_mode) {
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_ARRAY:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE_ARRAY:
            glDeleteTextures(1, m_object);
            return false;
        
        default: // Create Texture
            this->destroyInternal();
            GLTexture* tex0 = new GLTexture(m_ctx);
            tex0->m_pixel_type = m_pixel_type;
            // Use Internal Texture
            m_target = tex0;
            m_object = &tex0->m_tex;
            return true;
    }
}

// -------------------------------------
// OpenGL Renderbuffer: External Texture
// -------------------------------------

void GLRenderBuffer::useTexture(GPUTexture* texture) {
    m_ctx->gl__makeCurrent();
    this->destroyInternal();

    GLTexture* tex0 = dynamic_cast<GLTexture*>(texture);
    if (tex0->m_pixel_type != m_pixel_type) {
        GPUReport::error("mismatch texture pixel type for framebuffer");
        return;
    }

    // Use External Texture
    m_target = tex0;
    m_object = &tex0->m_tex;
    this->prepareExternal();
}

// -------------------------------------
// OpenGL Renderbuffer: Internal Texture
// -------------------------------------

void GLRenderBuffer::createTexture(int w, int h, int levels, int samples) {
    m_ctx->gl__makeCurrent();
    levels = levels_power_of_two(w, h, levels);
    samples = (samples > 0) ? next_power_of_two(samples) : 1;
    // Check Needs Re-create
    this->prepareInternal();

    // Generate New Texture
    glGenTextures(1, m_object);
    if (samples <= 1) {
        glBindTexture(GL_TEXTURE_2D, *(m_object));
        glTexStorage2D(GL_TEXTURE_2D, levels, toValue(m_pixel_type), w, h);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Set RenderBuffer Modes
        m_target->m_tex_target = GL_TEXTURE_2D;
        m_mode = GPURenderBufferMode::RENDERBUFFER_TEXTURE;
    } else {
        // Check if Multisamples are supported
        if (!GLAD_GL_ARB_texture_storage_multisample) {
            GPUReport::error("multisample textures are not supported");
            destroyInternal();
            return;
        }

        // Create Multisample Texture
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *(m_object));
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE,
            samples, toValue(m_pixel_type), w, h, 0);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

        // Set RenderBuffer Modes
        m_target->m_tex_target = GL_TEXTURE_2D_MULTISAMPLE;
        m_mode = GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE;
        levels = 1;
    }

    // Describe RenderBuffer Texture
    m_target->m_levels = levels;
    m_target->m_width = w;
    m_target->m_height = h;
    m_target->m_depth = 1;
    m_samples = samples;
}

void GLRenderBuffer::createTextureArray(int w, int h, int layers, int levels, int samples) {
    m_ctx->gl__makeCurrent();
    levels = levels_power_of_two(w, h, levels);
    samples = (samples > 0) ? next_power_of_two(samples) : 1;
    // Check Texture Recreate
    this->prepareInternal();

    // Generate New Texture
    glGenTextures(1, m_object);
    if (samples <= 1) {
        glBindTexture(GL_TEXTURE_2D_ARRAY, *(m_object));
        glTexStorage3D(GL_TEXTURE_2D_ARRAY,
            levels, toValue(m_pixel_type), w, h, layers);
        glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

        // Set RenderBuffer Modes
        m_target->m_tex_target = GL_TEXTURE_2D_ARRAY;
        m_mode = GPURenderBufferMode::RENDERBUFFER_TEXTURE;
    } else {
        // Check if Multisamples are supported
        if (!GLAD_GL_ARB_texture_storage_multisample) {
            GPUReport::error("multisample textures are not supported");
            destroyInternal();
            return;
        }

        // Create Multisample Texture
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, *(m_object));
        glTexStorage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
            samples, toValue(m_pixel_type), w, h, layers, 0);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, 0);

        // Set RenderBuffer Modes
        m_target->m_tex_target = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
        m_mode = GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE;
        levels = 1;
    }

    // Describe RenderBuffer Texture
    m_target->m_levels = levels;
    m_target->m_width = w;
    m_target->m_height = h;
    m_target->m_depth = layers;
    m_samples = samples;
}

// ------------------------------
// OpenGL Renderbuffer: Offscreen
// ------------------------------

void GLRenderBuffer::createOffscreen(int w, int h, int samples) {
    m_ctx->gl__makeCurrent();
    this->destroyInternal();

    m_object = &m_tex;
    glGenRenderbuffers(1, m_object);
    glBindRenderbuffer(GL_RENDERBUFFER, *(m_object));
    if (samples <= 1) glRenderbufferStorage(GL_RENDERBUFFER, toValue(m_pixel_type), w, h);
    else glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, toValue(m_pixel_type), w, h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    m_width = w;
    m_height = h;
    m_samples = samples;
    m_mode = GPURenderBufferMode::RENDERBUFFER_OFFSCREEN;
}

// ------------------------------
// OpenGL Renderbuffer Attributes
// ------------------------------

GPUTexture* GLRenderBuffer::getTexture() {
    m_ctx->gl__makeCurrent();

    switch (m_mode) {
        case GPURenderBufferMode::RENDERBUFFER_UNDEFINED:
            GPUReport::error("cannot get texture of undefined renderbuffer");
            return nullptr;
        case GPURenderBufferMode::RENDERBUFFER_OFFSCREEN:
            GPUReport::error("cannot get texture of offscreen renderbuffer");
            return nullptr;

        default: // Return Texture
            return m_target;
    }
}

GPUTextureSize GLRenderBuffer::getSize() {
    m_ctx->gl__makeCurrent();
    GPUTextureSize size = {0, 0};

    switch (m_mode) {
        case GPURenderBufferMode::RENDERBUFFER_UNDEFINED:
            GPUReport::warning("cannot get dimensions of undefined renderbuffer");
            return size;
        case GPURenderBufferMode::RENDERBUFFER_OFFSCREEN:
            size.width = m_width;
            size.width = m_height;
            break;

        default: {
            GLTexture* target = m_target;
            size.width = target->m_width;
            size.height = target->m_height;
        }
    }

    // Return Dimensions
    return size;
}

int GLRenderBuffer::getWidth() {
    return this->getSize().width;
}

int GLRenderBuffer::getHeight() {
    return this->getSize().height;
}

int GLRenderBuffer::getLayers() {
    m_ctx->gl__makeCurrent();

    switch (m_mode) {
        case GPURenderBufferMode::RENDERBUFFER_UNDEFINED:
            GPUReport::warning("cannot get layers of undefined renderbuffer");
            return 0;
        case GPURenderBufferMode::RENDERBUFFER_OFFSCREEN:
            return 1;

        default: // Return Texture Layers
            return m_target->m_depth;
    }
}

int GLRenderBuffer::getDepth() {
    return getLayers();
}

int GLRenderBuffer::getSamples() {
    m_ctx->gl__makeCurrent();
    if (m_mode == GPURenderBufferMode::RENDERBUFFER_UNDEFINED)
        GPUReport::warning("cannot get samples of undefined renderbuffer");

    // Return Texture Samples
    return m_samples;
}
