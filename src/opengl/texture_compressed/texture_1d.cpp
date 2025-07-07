// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "../nogpu/opengl_compressed.h"
#include "../nogpu/opengl_context.h"
#include "../glad/glad.h"

// -----------------------
// Texture 1D: Constructor
// -----------------------

GLCompressed1D::GLCompressed1D(
    GLContext* ctx,
    GPUTextureCompressedType type,
    GPUTexturePixelFormat format) : GLTexture(ctx) {
        m_compressed_type = type;
        m_pixel_format = format;
        m_tex_target = GL_TEXTURE_1D;
}

// -------------------------------
// Texture 1D: Buffer Manipulation
// -------------------------------

void GLCompressed1D::allocate(int size, int levels) {
    m_ctx->gl__makeCurrent();

    this->generateTexture();
    GLenum target = m_tex_target;
    glTexStorage1D(target, levels, toValue(m_pixel_type), size);

    // Check Allocation Error
    GLenum error = glGetError();
    switch (error) {
        case GL_INVALID_ENUM:
            GPULogger::error("invalid pixel type for 1D %p", this);
        case GL_INVALID_OPERATION:
            GPULogger::error("invalid levels count for 1D %p", this);
        case GL_INVALID_VALUE:
            GPULogger::error("invalid size for 1D %p", this);
    }

    // Check Texture Errors
    if (error != GL_NO_ERROR)
        return;

    // Set Texture Dimensions
    m_width = size;
    m_height = 1;
    m_depth = 1;
}

void GLCompressed1D::upload(int x, int size, int level, void* data, int bytes) {
    m_ctx->gl__makeCurrent();

    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glCompressedTexSubImage1D(target, level, x, size,
        toValue(m_compressed_type), bytes, data);

    // Check Uploading Error
    GLenum error = glGetError();
    switch (error) {
        case GL_INVALID_OPERATION:
            GPULogger::error("failed uploading pixels for 1D %p", this);
        case GL_INVALID_VALUE:
            GPULogger::error("failed uploading parameters for 1D %p", this);
        case GL_INVALID_ENUM:
            GPULogger::error("invalid pixel format/type for 1D %p", this);
    }
}

// -----------------------------------------
// Texture 1D: Buffer Manipulation using PBO
// -----------------------------------------

void GLCompressed1D::unpack(int x, int size, int level, GPUBuffer *pbo, int bytes, int offset) {
    m_ctx->gl__makeCurrent();

    // Copy PBO Pixels to Texture
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf->m_vbo);
    this->upload(x, size, level, reinterpret_cast<void*>(offset), bytes);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
