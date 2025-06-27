// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "../nogpu/opengl_texture.h"
#include "../nogpu/opengl_context.h"
#include "../glad/glad.h"
#include <climits>

// -----------------------
// Texture 2D: Constructor
// -----------------------

GLTexture2D::GLTexture2D(
    GLContext* ctx,
    GPUTexturePixelType type,
    GPUTexturePixelFormat format) : GLTexture(ctx) {
        m_pixel_type = type;
        m_pixel_format = format;
        m_tex_target = GL_TEXTURE_2D;
        // Check Depth Stencil Transfer Type
        if (type == GPUTexturePixelType::TEXTURE_PIXEL_DEPTH24_STENCIL8)
            m_transfer_type = GPUTextureTransferType::TEXTURE_TRANSFER_DEPTH24_STENCIL8;
}

GPUTexture2DMode GLTexture2D::getMode() {
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

void GLTexture2D::setMode(GPUTexture2DMode mode) {
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

void GLTexture2D::allocate(GPUTexture2DMode mode, int w, int h, int levels) {
    m_ctx->gl__makeCurrent();

    this->setMode(mode);
    this->generateTexture();
    GLenum target = m_tex_target;
    glTexStorage2D(target, levels, toValue(m_pixel_type), w, h);

    // Check Allocation Error
    GLenum error = glGetError();
    switch (error) {
        case GL_INVALID_ENUM:
            GPULogger::error("invalid pixel type for 2D %p", this); return;
        case GL_INVALID_OPERATION:
            GPULogger::error("invalid levels count for 2D %p", this); return;
        case GL_INVALID_VALUE:
            GPULogger::error("invalid size for 2D %p", this); return;
    }

    // Set Texture Dimensions
    m_width = w;
    m_height = h;
}

void GLTexture2D::upload(int x, int y, int w, int h, int level, void* data) {
    m_ctx->gl__makeCurrent();

    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glTexSubImage2D(target, level, x, y, w, h,
        toValue(m_pixel_format), toValue(m_transfer_type), data);

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

void GLTexture2D::download(int x, int y, int w, int h, int level, void* data) {
    m_ctx->gl__makeCurrent();

    GLenum error = GL_NO_ERROR;
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);

    // Use Optimized glGetTextureSubImage if available
    if (GLAD_GL_ARB_get_texture_sub_image) {
        glGetTextureSubImage(m_tex, level,
            x, y, 0, w, h, 1,
            toValue(m_pixel_format),
            toValue(m_transfer_type),
            INT_MAX, data);
        error = glGetError();
    // Use Optimized glGetTexImage when full image
    } else if (x == 0 && y == 0 && w == m_width && h == m_height) {
        glGetTexImage(target, level,
            toValue(m_pixel_format),
            toValue(m_transfer_type),
            data);
        error = glGetError();
    // Use Framebuffer Trick for Old Devices
    } else if (m_tex_target == GL_TEXTURE_2D) {
        error = download__hacky2D(x, y, w, h, level, data);
    } else if (m_tex_target == GL_TEXTURE_1D_ARRAY) {
        error = download__hacky1DArray(x, y, w, h, level, data);
    }

    // Check Succesfull
    if (error != GL_NO_ERROR)
        GPULogger::error("failed downloading pixels from 2D %p", this);
}

// --------------------------------
// Texture 2D: Download Workarounds
// --------------------------------

GLenum GLTexture2D::download__hacky2D(int x, int y, int w, int h, int level, void* data) {
    if (!m_tex_fbo) glGenFramebuffers(1, &m_tex_fbo);

    GLenum attachment = toHackyFramebufferAttachmentType(m_pixel_format);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_tex_fbo);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER,
        attachment, m_tex_target, m_tex, level);

    // Check if Texture and Framebuffer is valid to hacky read
    if (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &m_tex_fbo);
        return GL_INVALID_OPERATION;
    }

    GLint read = 0;
    GLenum error = GL_NO_ERROR;
    // Read Framebuffer Pixels
    glGetIntegerv(GL_READ_BUFFER, &read);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(x, y, w, h,
        toValue(m_pixel_format),
        toValue(m_transfer_type),
        data);

    error = glGetError();
    glReadBuffer(read);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    return error;
}

GLenum GLTexture2D::download__hacky1DArray(int x, int y, int w, int h, int level, void* data) {
    if (!m_tex_fbo) glGenFramebuffers(1, &m_tex_fbo);

    // Check if Texture and Framebuffer is valid to hacky read
    GLenum attachment = toHackyFramebufferAttachmentType(m_pixel_format);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_tex_fbo);
    glFramebufferTextureLayer(GL_READ_FRAMEBUFFER,
        attachment, m_tex, level, y);

    // Check if Texture and Framebuffer is valid to hacky read
    if (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &m_tex_fbo);
        return GL_INVALID_OPERATION;
    }

    GLint read = 0;
    GLenum error = GL_NO_ERROR;
    glGetIntegerv(GL_READ_BUFFER, &read);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    // Read Framebuffer for Layers
    for (int i = 0; i < h; i++) {
        glFramebufferTextureLayer(GL_READ_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0, m_tex, level, y + i);
        glReadPixels(x, 0, w, 1,
            toValue(m_pixel_format),
            toValue(m_transfer_type),
            data);

        // Check Read Error
        error = glGetError();
        if (error != GL_NO_ERROR)
            break;
    }

    glReadBuffer(read);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    return error;
}

// -----------------------------------------
// Texture 2D: Buffer Manipulation using PBO
// -----------------------------------------

void GLTexture2D::unpack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int offset) {
    m_ctx->gl__makeCurrent();

    // Copy PBO Pixels to Texture
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf->m_vbo);
    this->upload(x, y, w, h, level, reinterpret_cast<void*>(offset));
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void GLTexture2D::pack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int offset) {
    m_ctx->gl__makeCurrent();

    // Copy Texture Pixels to PBO
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, buf->m_vbo);
    this->download(x, y, w, h, level, reinterpret_cast<void*>(offset));
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}
