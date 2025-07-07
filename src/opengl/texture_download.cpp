// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/texture.h"
#include "private/glad.h"

static GLenum valueAttachmentType(GPUTexturePixelFormat format) {
    switch (format) {
        case GPUTexturePixelFormat::TEXTURE_FORMAT_DEPTH_COMPONENT:
            return GL_DEPTH_ATTACHMENT;
        case GPUTexturePixelFormat::TEXTURE_FORMAT_DEPTH_STENCIL:
            return GL_DEPTH_STENCIL_ATTACHMENT;
        default: // RGBA Color Fallback
            return GL_COLOR_ATTACHMENT0;
    }
}

static int valueTransferBytes(GPUTextureTransferType type, GPUTexturePixelFormat format) {
    int bytes = 0;

    switch (type) {
        case GPUTextureTransferType::TEXTURE_TRANSFER_UNSIGNED_BYTE:
        case GPUTextureTransferType::TEXTURE_TRANSFER_BYTE:
            bytes = (int) sizeof(unsigned char);
        case GPUTextureTransferType::TEXTURE_TRANSFER_UNSIGNED_SHORT:
        case GPUTextureTransferType::TEXTURE_TRANSFER_SHORT:
            bytes = (int) sizeof(unsigned short);
        case GPUTextureTransferType::TEXTURE_TRANSFER_FLOAT:
        case GPUTextureTransferType::TEXTURE_TRANSFER_DEPTH24_STENCIL8:
        case GPUTextureTransferType::TEXTURE_TRANSFER_UNSIGNED_INT:
        case GPUTextureTransferType::TEXTURE_TRANSFER_INT:
            bytes = (int) sizeof(unsigned int);
    }

    switch (format) {
        case GPUTexturePixelFormat::TEXTURE_FORMAT_COMPRESSED:
        case GPUTexturePixelFormat::TEXTURE_FORMAT_DEPTH_COMPONENT:
        case GPUTexturePixelFormat::TEXTURE_FORMAT_DEPTH_STENCIL:
        case GPUTexturePixelFormat::TEXTURE_FORMAT_RED:
            bytes *= 1;
        case GPUTexturePixelFormat::TEXTURE_FORMAT_RG:
            bytes *= 2;
        case GPUTexturePixelFormat::TEXTURE_FORMAT_RGB:
        case GPUTexturePixelFormat::TEXTURE_FORMAT_BGR:
            bytes *= 3;
        case GPUTexturePixelFormat::TEXTURE_FORMAT_RGBA:
        case GPUTexturePixelFormat::TEXTURE_FORMAT_BGRA:
            bytes *= 4;
    }

    // Return Transfer Bytes
    return bytes;
}

// -------------------------------------
// Texture 2D: Download Compatibility 3D
// -------------------------------------

GLenum GLTexture::compatDownload3D(int x, int y, int z, int w, int h, int depth, int level, void* data) {
    if (!m_tex_fbo) glGenFramebuffers(1, &m_tex_fbo);

    GLenum attachment = valueAttachmentType(m_pixel_format);
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
    int transfer_bytes = valueTransferBytes(m_transfer_type, m_pixel_format);
    int layer_bytes = transfer_bytes * w * h;

    // Read Framebuffer for Layers
    for (int i = 0; i < depth; i++) {
        glFramebufferTextureLayer(GL_READ_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0, m_tex, level, z + i);
        glReadPixels(x, y, w, h,
            toValue(m_pixel_format),
            toValue(m_transfer_type),
            dst); dst += layer_bytes;

        // Check Read Error
        error = glGetError();
        if (error != GL_NO_ERROR)
            break;
    }

    glReadBuffer(read);
    return error;
}

// -------------------------------------
// Texture 2D: Download Compatibility 2D
// -------------------------------------

GLenum GLTexture::compatDownload2D(int x, int y, int w, int h, int level, void* data) {
    if (!m_tex_fbo) glGenFramebuffers(1, &m_tex_fbo);

    GLenum attachment = valueAttachmentType(m_pixel_format);
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
        toValue(m_pixel_format),
        toValue(m_transfer_type),
        data);

    error = glGetError();
    glReadBuffer(read);
    return error;
}
