// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "../private/compressed.h"
#include "../private/context.h"
#include "../private/glad.h"

// -----------------------
// Texture 2D: Constructor
// -----------------------

GLCompressed2D::GLCompressed2D(GLContext* ctx, GPUTextureCompressedType type) : GLTexture(ctx) {
        m_pixel_type = GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED;
        m_transfer_format = GPUTextureTransferFormat::TEXTURE_FORMAT_COMPRESSED;
        m_transfer_size = GPUTextureTransferSize::TEXTURE_SIZE_COMPRESSED;
        m_tex_target = GL_TEXTURE_2D;
        m_compressed_type = type;
}

GPUTexture2DMode GLCompressed2D::getMode() {
    m_ctx->gl__makeCurrent();

    switch (m_tex_target) {
        case GL_TEXTURE_2D:
            return GPUTexture2DMode::TEXTURE_2D;
        case GL_TEXTURE_RECTANGLE:
            return GPUTexture2DMode::TEXTURE_2D_RECTANGLE;
        case GL_TEXTURE_1D_ARRAY:
            return GPUTexture2DMode::TEXTURE_1D_ARRAY;
    }

    // Return Fallback Value
    return GPUTexture2DMode::TEXTURE_2D;
}

void GLCompressed2D::setMode(GPUTexture2DMode mode) {
    switch (mode) {
        case GPUTexture2DMode::TEXTURE_2D:
            m_tex_target = GL_TEXTURE_2D; break;
        case GPUTexture2DMode::TEXTURE_2D_RECTANGLE:
            m_tex_target = GL_TEXTURE_RECTANGLE; break;
        case GPUTexture2DMode::TEXTURE_1D_ARRAY:
            m_tex_target = GL_TEXTURE_1D_ARRAY; break;
    }
}

// -------------------------------
// Texture 2D: Buffer Manipulation
// -------------------------------

void GLCompressed2D::allocate(GPUTexture2DMode mode, int w, int h, int levels) {
    m_ctx->gl__makeCurrent();

    this->setMode(mode);
    this->generateTexture();
    GLenum target = m_tex_target;
    glTexStorage2D(target, levels, toValue(m_pixel_type), w, h);

    // Check Allocation Error
    GLenum error = glGetError();
    switch (error) {
        case GL_INVALID_ENUM:
            GPULogger::error("invalid pixel type for 2D %p", this);
        case GL_INVALID_OPERATION:
            GPULogger::error("invalid levels count for 2D %p", this);
        case GL_INVALID_VALUE:
            GPULogger::error("invalid size for 2D %p", this);
    }

    // Check Texture Errors
    if (error != GL_NO_ERROR)
        return;

    // Set Texture Dimensions
    m_width = w;
    m_height = h;
    m_depth = 1;
}

void GLCompressed2D::upload(int x, int y, int w, int h, int level, void* data, int bytes) {
    m_ctx->gl__makeCurrent();

    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glCompressedTexSubImage2D(target, level, x, y, w, h,
        toValue(m_compressed_type), bytes, data);

    // Check Uploading Error
    GLenum error = glGetError();
    switch (error) {
        case GL_INVALID_OPERATION:
            GPULogger::error("failed uploading pixels for 2D %p", this);
        case GL_INVALID_VALUE:
            GPULogger::error("failed uploading parameters for 2D %p", this);
        case GL_INVALID_ENUM:
            GPULogger::error("invalid pixel format/type for 2D %p", this);
    }
}

// -----------------------------------------
// Texture 2D: Buffer Manipulation using PBO
// -----------------------------------------

void GLCompressed2D::unpack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int bytes, int offset) {
    m_ctx->gl__makeCurrent();

    // Copy PBO Pixels to Texture
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf->m_vbo);
    this->upload(x, y, w, h, level, reinterpret_cast<void*>(offset), bytes);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
