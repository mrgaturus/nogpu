// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/texture.h"
#include "private/glad.h"

static GLenum valueAttachmentType(GPUTexturePixelType type) {
    switch (type) {
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT16:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT24:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT32:
            return GL_DEPTH_ATTACHMENT;
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH24_STENCIL8:
            return GL_DEPTH_STENCIL_ATTACHMENT;
        default: // RGBA Color Attachment
            return GL_COLOR_ATTACHMENT0;
    }
}

// -------------------------------------
// Texture 2D: Download Compatibility 3D
// -------------------------------------

GLenum GLTexture::compatDownload3D(int x, int y, int z, int w, int h, int depth, int level, void* data) {
    if (!m_tex_fbo) glGenFramebuffers(1, &m_tex_fbo);

    GLenum attachment = valueAttachmentType(m_pixel_type);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_tex_fbo);
    glFramebufferTextureLayer(GL_READ_FRAMEBUFFER,
        attachment, m_tex, level, z);

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

    unsigned char* dst = static_cast<unsigned char*>(data);
    int transfer_bytes = computeBytesPerPixel(m_transfer_format, m_transfer_size);
    int layer_bytes = transfer_bytes * w * h;

    // Read Framebuffer for Layers
    for (int i = 0; i < depth; i++) {
        glFramebufferTextureLayer(GL_READ_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0, m_tex, level, z + i);
        glReadPixels(x, y, w, h,
            toValue(m_transfer_format),
            toValue(m_transfer_size),
            dst); dst += layer_bytes;
    }

    // Restore Read Buffer
    glReadBuffer(read);
}

// -------------------------------------
// Texture 2D: Download Compatibility 2D
// -------------------------------------

GLenum GLTexture::compatDownload2D(int x, int y, int w, int h, int level, void* data) {
    if (!m_tex_fbo) glGenFramebuffers(1, &m_tex_fbo);

    GLenum attachment = valueAttachmentType(m_pixel_type);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_tex_fbo);
    glFramebufferTexture(GL_READ_FRAMEBUFFER,
        attachment, m_tex, level);

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
        toValue(m_transfer_format),
        toValue(m_transfer_size),
        data);

    // Restore Read Buffer
    glReadBuffer(read);
}
