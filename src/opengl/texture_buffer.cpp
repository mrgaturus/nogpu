// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/texture.h"
#include "private/context.h"
#include "private/glad.h"

void GLTextureBuffer::updateTexture() {
    glBindTexture(GL_TEXTURE_BUFFER, m_tex);
    if (m_size > 0) glTexBufferRange(GL_TEXTURE_BUFFER,
        toValue(m_pixel_type), m_buffer->m_vbo, m_offset, m_size);
    else glTexBuffer(GL_TEXTURE_BUFFER,
        toValue(m_pixel_type), m_buffer->m_vbo);
    glBindTexture(GL_TEXTURE_BUFFER, 0);
}

// ---------------------------
// Texture Buffer: Constructor
// ---------------------------

GLTextureBuffer::GLTextureBuffer(GLContext* ctx, GLBuffer* buffer, GPUTexturePixelType type) {
    ctx->makeCurrentTexture(this);

    // Check if driver has Texture Buffer
    if (!GLAD_GL_ARB_texture_buffer_range) {
        GPUReport::error("texture buffer is not supported");
        delete this;
        return;
    // Check Texture Pixel Format
    } else if (!canTextureBuffer(type)) {
        GPUReport::error("invalid pixel format for texture buffer %p", this);
        delete this;
        return;
    }

    // Define Texture Buffer
    m_pixel_type = type;
    m_buffer = buffer;
    m_ctx = ctx;

    // Create Texture Buffer
    glGenTextures(1, &m_tex);
    this->updateTexture();
}

void GLTextureBuffer::destroy() {
    m_ctx->makeCurrentTexture(this);

    // Dealloc Texture Buffer Object
    glBindTexture(GL_TEXTURE_BUFFER, 0);
    glDeleteTextures(1, &m_tex);
    // Dealloc Object
    delete this;
}

// ----------------------------
// Texture Buffer: Manipulation
// ----------------------------

void GLTextureBuffer::setType(GPUTexturePixelType type) {
    m_ctx->makeCurrentTexture(this);
    // Check Valid Texture Pixel Type
    if (!canTextureBuffer(type)) {
        GPUReport::error("invalid pixel format for texture buffer %p", this);
        return;
    }

    // Update Texture Type
    m_pixel_type = type;
    this->updateTexture();
}

void GLTextureBuffer::setBuffer(GPUBuffer* buffer) {
    m_ctx->makeCurrentTexture(this);

    // Update Texture Buffer
    m_buffer = static_cast<GLBuffer*>(buffer);
    this->updateTexture();
}

// ----------------------------------
// Texture Buffer: Range Manipulation
// ----------------------------------

void GLTextureBuffer::setRange(GPUTextureBufferRange range) {
    m_ctx->makeCurrentTexture(this);
    if (range.size <= 0) {
        this->clearRange();
        return;
    }

    int aligment = 0;
    // Align Range Offset to Valid Alignment
    glGetIntegerv(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, &aligment);
    range.offset &= ~(aligment - 1);

    // Change Current Range
    m_offset = range.offset;
    m_size = range.size;
    // Update Texture Buffer
    this->updateTexture();
}

void GLTextureBuffer::clearRange() {
    m_ctx->makeCurrentTexture(this);
    // Clear Buffer Range
    m_offset = 0;
    m_size = 0;

    // Update Texture Buffer
    this->updateTexture();
}

// ------------------------------------
// Texture Buffer: Manipulation Getters
// ------------------------------------

GPUTexturePixelType GLTextureBuffer::getType() {
    m_ctx->makeCurrentTexture(this);
    return m_pixel_type;
}

GPUBuffer* GLTextureBuffer::getBuffer() {
    m_ctx->makeCurrentTexture(this);
    return m_buffer;
}
