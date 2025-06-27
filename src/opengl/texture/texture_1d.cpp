// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "../nogpu/opengl_texture.h"
#include "../nogpu/opengl_context.h"
#include "../glad/glad.h"
#include <climits>

// -----------------------
// Texture 1D: Constructor
// -----------------------

GLTexture1D::GLTexture1D(
    GLContext* ctx,
    GPUTexturePixelType type,
    GPUTexturePixelFormat format) : GLTexture(ctx) {
        m_pixel_type = type;
        m_pixel_format = format;
        m_tex_target = GL_TEXTURE_1D;
        // Check Depth Stencil Transfer Type
        if (type == GPUTexturePixelType::TEXTURE_PIXEL_DEPTH24_STENCIL8)
            m_transfer_type = GPUTextureTransferType::TEXTURE_TRANSFER_DEPTH24_STENCIL8;
}

// -------------------------------
// Texture 1D: Buffer Manipulation
// -------------------------------

void GLTexture1D::allocate(int size, int levels) {
    m_ctx->gl__makeCurrent();

    this->generateTexture();
    GLenum target = m_tex_target;
    glTexStorage1D(target, levels, toValue(m_pixel_type), size);

    // Check Allocation Error
    GLenum error = glGetError();
    switch (error) {
        case GL_INVALID_ENUM:
            GPULogger::error("invalid pixel type for 1D %p", this); return;
        case GL_INVALID_OPERATION:
            GPULogger::error("invalid levels count for 1D %p", this); return;
        case GL_INVALID_VALUE:
            GPULogger::error("invalid size for 1D %p", this); return;
    }

    // Set Texture Dimensions
    m_width = size;
    m_height = 1;
}

void GLTexture1D::upload(int x, int size, int level, void* data) {
    m_ctx->gl__makeCurrent();

    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glTexSubImage1D(target, level, x, size,
        toValue(m_pixel_format), toValue(m_transfer_type), data);

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

void GLTexture1D::download(int x, int size, int level, void* data) {
    m_ctx->gl__makeCurrent();

    GLenum error = GL_NO_ERROR;
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);

    // Use Optimized glGetTextureSubImage if available
    if (GLAD_GL_ARB_get_texture_sub_image) {
        glGetTextureSubImage(m_tex, level,
            x, 0, 0, size, 1, 1,
            toValue(m_pixel_format),
            toValue(m_transfer_type),
            INT_MAX, data);
        error = glGetError();
    // Use Optimized glGetTexImage when full image
    } else if (x == 0 && size == m_width) {
        glGetTexImage(target, level,
            toValue(m_pixel_format),
            toValue(m_transfer_type),
            data);
        error = glGetError();
    // Use Framebuffer Trick for Old Devices
    } else { 
        error = download__hacky1D(
            x, size, level, data);     
    }

    // Check Succesfull
    if (error != GL_NO_ERROR)
        GPULogger::error("failed downloading pixels from 1D %p", this);
}

// --------------------------------
// Texture 1D: Download Workarounds
// --------------------------------

GLenum GLTexture1D::download__hacky1D(int x, int size, int level, void* data) {
    if (!m_tex_fbo) glGenFramebuffers(1, &m_tex_fbo);

    GLenum attachment = toHackyFramebufferAttachmentType(m_pixel_format);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_tex_fbo);
    glFramebufferTexture1D(GL_READ_FRAMEBUFFER,
        attachment, GL_TEXTURE_1D, m_tex, level);

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
    glReadPixels(x, 0, size, 1,
        toValue(m_pixel_format),
        toValue(m_transfer_type),
        data);

    // Return Error Check
    error = glGetError();
    glReadBuffer(read);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    return error;
}

// -----------------------------------------
// Texture 1D: Buffer Manipulation using PBO
// -----------------------------------------

void GLTexture1D::unpack(int x, int size, int level, GPUBuffer *pbo, int offset) {
    m_ctx->gl__makeCurrent();

    // Copy PBO Pixels to Texture
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf->m_vbo);
    this->upload(x, size, level, reinterpret_cast<void*>(offset));
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void GLTexture1D::pack(int x, int size, int level, GPUBuffer *pbo, int offset) {
    m_ctx->gl__makeCurrent();

    // Copy Texture Pixels to PBO
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, buf->m_vbo);
    this->download(x, size, level, reinterpret_cast<void*>(offset));
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}
