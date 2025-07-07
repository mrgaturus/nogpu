// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "../private/texture.h"
#include "../private/context.h"
#include "../private/glad.h"
#include <climits>

// -----------------------
// Texture 2D: Constructor
// -----------------------

GLTexture3D::GLTexture3D(
    GLContext* ctx, GPUTexturePixelType type) : GLTexture(ctx) {
        m_pixel_type = type;
        m_tex_target = GL_TEXTURE_3D;
}

GPUTexture3DMode GLTexture3D::getMode() {
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

void GLTexture3D::setMode(GPUTexture3DMode mode) {
    switch (mode) {
        case GPUTexture3DMode::TEXTURE_3D:
            m_tex_target = GL_TEXTURE_3D; break;
        case GPUTexture3DMode::TEXTURE_2D_ARRAY:
            m_tex_target = GL_TEXTURE_2D_ARRAY; break;
    }
}

// -------------------------------
// Texture 2D: Buffer Manipulation
// -------------------------------

void GLTexture3D::allocate(GPUTexture3DMode mode, int w, int h, int depth, int levels) {
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

void GLTexture3D::upload(int x, int y, int z, int w, int h, int depth, int level, void* data) {
    m_ctx->gl__makeCurrent();

    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glTexSubImage3D(target, level, x, y, z, w, h, depth,
        toValue(m_transfer_format),
        toValue(m_transfer_size), data);

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

void GLTexture3D::download(int x, int y, int z, int w, int h, int depth, int level, void* data) {
    m_ctx->gl__makeCurrent();

    GLenum error = GL_NO_ERROR;
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);

    // Use Optimized glGetTextureSubImage if available
    if (GLAD_GL_ARB_get_texture_sub_image) {
        glGetTextureSubImage(m_tex, level,
            x, y, z, w, h, depth,
            toValue(m_transfer_format),
            toValue(m_transfer_size),
            INT_MAX, data);
        error = glGetError();
    // Use Optimized glGetTexImage when full image
    } else if (x == 0 && y == 0 && z == 0 && w == m_width && h == m_height && depth == m_depth) {
        glGetTexImage(target, level,
            toValue(m_transfer_format),
            toValue(m_transfer_size),
            data);
        error = glGetError();
    // Use Framebuffer Trick for Old Devices
    } else {
        error = compatDownload3D(x, y, z, w, h, depth, level, data);
    }

    // Check Succesfull
    if (error != GL_NO_ERROR)
        GPULogger::error("failed downloading pixels from 2D %p", this);
}

// -----------------------------------------
// Texture 3D: Buffer Manipulation using PBO
// -----------------------------------------

void GLTexture3D::unpack(int x, int y, int z, int w, int h, int depth, int level, GPUBuffer *pbo, int offset) {
    m_ctx->gl__makeCurrent();

    // Copy PBO Pixels to Texture
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf->m_vbo);
    this->upload(x, y, z, w, h, depth, level,
        reinterpret_cast<void*>(offset));
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void GLTexture3D::pack(int x, int y, int z, int w, int h, int depth, int level, GPUBuffer *pbo, int offset) {
    m_ctx->gl__makeCurrent();

    // Copy Texture Pixels to PBO
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, buf->m_vbo);
    this->download(x, y, z, w, h, depth, level,
        reinterpret_cast<void*>(offset));
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}
