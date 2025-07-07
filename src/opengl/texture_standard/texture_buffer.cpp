// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "../private/texture.h"
#include "../private/context.h"
#include "../private/glad.h"

// ---------------------------
// Texture Buffer: Constructor
// ---------------------------

GLTextureBuffer::GLTextureBuffer(GLContext* ctx, GPUTexturePixelType type) {
    ctx->gl__makeCurrent();
    // Check Valid Texture Pixel Types
    if (static_cast<int>(type) > static_cast<int>(GPUTexturePixelType::TEXTURE_PIXEL_RGBA32UI)) {
        GPULogger::error("invalid pixel format for texture buffer %p", this);
        delete this;
        return;
    }

    // Define Texture Buffer
    m_pixel_type = type;
    m_buffer = new GLBuffer(ctx);
    m_ctx = ctx;
}

void GLTextureBuffer::destroy() {
    m_ctx->gl__makeCurrent();
    m_buffer->destroy();

    // Dealloc Object
    delete this;
}

// ----------------------------
// Texture Buffer: Manipulation
// ----------------------------

void GLTextureBuffer::setType(GPUTexturePixelType type) {
    m_ctx->gl__makeCurrent();
    // Check Valid Texture Pixel Types and Change Type
    if (static_cast<int>(type) > static_cast<int>(GPUTexturePixelType::TEXTURE_PIXEL_RGBA32UI)) {
        GPULogger::error("invalid pixel format for texture buffer %p", this);
        return;
    }

    // Change Pixel Type
    m_pixel_type = type;
}

GPUTexturePixelType GLTextureBuffer::getType() {
    m_ctx->gl__makeCurrent();
    return m_pixel_type;
}

GPUBuffer* GLTextureBuffer::getBuffer() {
    m_ctx->gl__makeCurrent();
    return m_buffer;
}
