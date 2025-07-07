// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "../private/compressed.h"
#include "../private/context.h"
#include "../private/glad.h"

// -----------------------
// Texture 3D: Constructor
// -----------------------

GLCompressed3D::GLCompressed3D(GLContext* ctx, GPUTextureCompressedType type) : GLTexture(ctx) {
        m_pixel_type = GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED;
        m_transfer_format = GPUTextureTransferFormat::TEXTURE_FORMAT_COMPRESSED;
        m_transfer_size = GPUTextureTransferSize::TEXTURE_SIZE_COMPRESSED;
        m_tex_target = GL_TEXTURE_3D;
        m_compressed_type = type;
}

GPUTexture3DMode GLCompressed3D::getMode() {
    m_ctx->gl__makeCurrent();

    switch (m_tex_target) {
        case GL_TEXTURE_3D:
            return GPUTexture3DMode::TEXTURE_3D;
        case GL_TEXTURE_2D_ARRAY:
            return GPUTexture3DMode::TEXTURE_2D_ARRAY;
    }

    // Return Fallback Value
    return GPUTexture3DMode::TEXTURE_3D;
}

void GLCompressed3D::setMode(GPUTexture3DMode mode) {
    switch (mode) {
        case GPUTexture3DMode::TEXTURE_3D:
            m_tex_target = GL_TEXTURE_3D; break;
        case GPUTexture3DMode::TEXTURE_2D_ARRAY:
            m_tex_target = GL_TEXTURE_2D_ARRAY; break;
    }
}

// -------------------------------
// Texture 3D: Buffer Manipulation
// -------------------------------

void GLCompressed3D::allocate(GPUTexture3DMode mode, int w, int h, int depth, int levels) {
    m_ctx->gl__makeCurrent();

    this->setMode(mode);
    this->generateTexture();
    GLenum target = m_tex_target;
    glTexStorage3D(target, levels, toValue(m_pixel_type), w, h, depth);

    // Check Allocation Error
    GLenum error = glGetError();
    switch (error) {
        case GL_INVALID_ENUM:
            GPULogger::error("invalid pixel type for 3D %p", this);
        case GL_INVALID_OPERATION:
            GPULogger::error("invalid levels count for 3D %p", this);
        case GL_INVALID_VALUE:
            GPULogger::error("invalid size for 3D %p", this);
    }

    // Check Texture Errors
    if (error != GL_NO_ERROR)
        return;

    // Set Texture Dimensions
    m_width = w;
    m_height = h;
    m_depth = depth;
}

void GLCompressed3D::upload(int x, int y, int z, int w, int h, int depth, int level, void* data, int bytes) {
    m_ctx->gl__makeCurrent();

    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glCompressedTexSubImage3D(target, level, x, y, z, w, h, depth,
        toValue(m_compressed_type), bytes, data);

    // Check Uploading Error
    GLenum error = glGetError();
    switch (error) {
        case GL_INVALID_OPERATION:
            GPULogger::error("failed uploading pixels for 3D %p", this);
        case GL_INVALID_VALUE:
            GPULogger::error("failed uploading parameters for 3D %p", this);
        case GL_INVALID_ENUM:
            GPULogger::error("invalid pixel format/type for 3D %p", this);
    }
}

// -----------------------------------------
// Texture 3D: Buffer Manipulation using PBO
// -----------------------------------------

void GLCompressed3D::unpack(int x, int y, int z, int w, int h, int depth, int level, GPUBuffer *pbo, int bytes, int offset) {
    m_ctx->gl__makeCurrent();

    // Copy PBO Pixels to Texture
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf->m_vbo);
    this->upload(x, y, z, w, h, depth, level,
        reinterpret_cast<void*>(offset), bytes);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
