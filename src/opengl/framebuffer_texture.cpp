// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/framebuffer.h"
#include "private/context.h"
#include "private/glad.h"

// -----------------------------------------------
// OpenGL Renderbuffer: Internal Texture Offscreen
// -----------------------------------------------

void GLRenderBuffer::createOffscreen(int w, int h, int samples) {
    m_ctx->gl__makeCurrent();
    this->destroyInternal();
    GLenum error = GL_NO_ERROR;

    m_object = &m_tex;
    glGenRenderbuffers(1, m_object);
    glBindRenderbuffer(GL_RENDERBUFFER, *(m_object));
    if (samples <= 1) glRenderbufferStorage(GL_RENDERBUFFER, toValue(m_pixel_type), w, h);
    else glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, toValue(m_pixel_type), w, h);
    error = glGetError();
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Check Offscreen Error
    switch (error) {
        case GL_INVALID_VALUE:
            GPUReport::error("invalid offscreen texture dimensions or samples");
            return;
        case GL_OUT_OF_MEMORY:
            GPUReport::error("not enough vram to create offscreen texture");
            return;
        case GL_INVALID_ENUM:
            GPUReport::error("renderbuffer format is not renderable");
            return;
    }

    m_width = w;
    m_height = h;
    m_samples = samples;
    m_mode = GPURenderBufferMode::RENDERBUFFER_OFFSCREEN;
}

// ----------------------------------------
// OpenGL Renderbuffer: Internal Texture 2D
// ----------------------------------------

void GLRenderBuffer::createTexture(int w, int h, int levels, int samples) {
    m_ctx->gl__makeCurrent();
    levels = levels_power_of_two(w, h, levels);
    samples = (samples > 0) ? next_power_of_two(samples) : 1;
    // Check Texture Recreate
    this->prepareInternal();
    GLenum error = GL_NO_ERROR;

    // Generate New Texture
    glGenTextures(1, m_object);
    if (samples <= 1) {
        glBindTexture(GL_TEXTURE_2D, *(m_object));
        glTexStorage2D(GL_TEXTURE_2D, levels, toValue(m_pixel_type), w, h);
        error = glGetError();
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
        error = glGetError();
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

        // Set RenderBuffer Modes
        m_target->m_tex_target = GL_TEXTURE_2D_MULTISAMPLE;
        m_mode = GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE;
        levels = 1;
    }

    // Check Texture Error
    switch (error) {
        case GL_INVALID_VALUE:
            GPUReport::error("invalid texture dimensions or samples");
            return;
        case GL_OUT_OF_MEMORY:
            GPUReport::error("not enough vram to create texture");
            return;
        case GL_INVALID_ENUM:
            GPUReport::error("renderbuffer format is not renderable");
            return;
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
    GLenum error = GL_NO_ERROR;

    // Generate New Texture
    glGenTextures(1, m_object);
    if (samples <= 1) {
        glBindTexture(GL_TEXTURE_2D_ARRAY, *(m_object));
        glTexStorage3D(GL_TEXTURE_2D_ARRAY,
            levels, toValue(m_pixel_type), w, h, layers);
        error = glGetError();
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
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, *(m_object));
        glTexStorage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
            samples, toValue(m_pixel_type), w, h, layers, 0);
        error = glGetError();
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, 0);

        // Set RenderBuffer Modes
        m_target->m_tex_target = GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
        m_mode = GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE_ARRAY;
        levels = 1;
    }

    // Check Texture Error
    switch (error) {
        case GL_INVALID_VALUE:
            GPUReport::error("invalid texture array dimensions or samples");
            return;
        case GL_OUT_OF_MEMORY:
            GPUReport::error("not enough vram to create texture array");
            return;
        case GL_INVALID_ENUM:
            GPUReport::error("renderbuffer format is not renderable");
            return;
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
    m_ctx->gl__makeCurrent();
    levels = levels_power_of_two(w, h, levels);
    this->prepareInternal();
    GLenum error = GL_NO_ERROR;

    // Generate New Texture
    glGenTextures(1, m_object);
    glBindTexture(GL_TEXTURE_3D, *(m_object));
    glTexStorage3D(GL_TEXTURE_3D,
        levels, toValue(m_pixel_type), w, h, layers);
    error = glGetError();
    glBindTexture(GL_TEXTURE_3D, 0);
    // Set RenderBuffer Modes
    m_target->m_tex_target = GL_TEXTURE_3D;
    m_mode = GPURenderBufferMode::RENDERBUFFER_TEXTURE_3D;

    // Check Texture Error
    switch (error) {
        case GL_INVALID_VALUE:
            GPUReport::error("invalid texture 3D dimensions or samples");
            return;
        case GL_OUT_OF_MEMORY:
            GPUReport::error("not enough vram to create texture 3D");
            return;
        case GL_INVALID_ENUM:
            GPUReport::error("renderbuffer format is not renderable");
            return;
    }

    // Describe RenderBuffer Texture
    m_target->m_levels = levels;
    m_target->m_width = w;
    m_target->m_height = h;
    m_target->m_depth = layers;
    m_samples = 1;
}
