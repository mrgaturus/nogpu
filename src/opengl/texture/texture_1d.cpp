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
        
        // Check if texture 1D is supported
        if (!GPUDriver::checkFeature(GPUDriverFeature::DRIVER_TEXTURE_1D)) {
            GPULogger::error("texture 1d is not supported");
            delete this;
        }
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
    m_w = size;
    m_h = 1;
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
            GPULogger::error("failed uploading pixels for 1D %p", this); return;
        case GL_INVALID_VALUE:
            GPULogger::error("failed uploading parameters for 1D %p", this); return;
        case GL_INVALID_ENUM:
            GPULogger::error("invalid pixel format/type for 1D %p", this); return;
    }
}

void GLTexture1D::download(int x, int size, int level, void* data) {
    m_ctx->gl__makeCurrent();

    GLint read;
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

        // Check Successful
        error = glGetError();
        if (error != GL_NO_ERROR)
            goto DOWNLOAD_ERROR;
        return;
    // Use Optimized glGetTexImage when full image
    } else if (x == 0 && size == m_w) {
        glGetTexImage(target, level,
            toValue(m_pixel_format),
            toValue(m_transfer_type),
            data);

        // Check Successful
        error = glGetError();
        if (error != GL_NO_ERROR)
            goto DOWNLOAD_ERROR;
        return;
    }

    // Create Read Framebuffer
    if (!m_tex_fbo) {
        glGenFramebuffers(1, &m_tex_fbo);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_tex_fbo);
        glFramebufferTexture1D(GL_READ_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0, 
            GL_TEXTURE_1D, m_tex, 0);

        // Check if Texture and Framebuffer is valid to hacky read
        if (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &m_tex_fbo);
            goto DOWNLOAD_ERROR;
        }
    } 

    // Use Framebuffer Trick for Old Devices
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_tex_fbo);
    glGetIntegerv(GL_READ_BUFFER, &read);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(x, 0, size, 1,
        toValue(m_pixel_format),
        toValue(m_transfer_type),
        data);

    error = glGetError(); glReadBuffer(read);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    // Check Texture Reading Successful
    if (error != GL_NO_ERROR) {
        DOWNLOAD_ERROR: {
            GPULogger::error("failed downloading pixels from 1D %p", this);
        }
    }
}

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
