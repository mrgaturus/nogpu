// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/texture.h"
#include "private/glad.h"

// -----------------------------------
// OpenGL Texture: Download Attachment
// -----------------------------------

static GLenum downloadAttachmentType(GPUTexturePixelType type) {
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

// --------------------------------------
// OpenGL Texture: Download Compatibility
// --------------------------------------

void GLTexture::compatDownload3D(int x, int y, int z, int w, int h, int depth, int level, void* data) {
    if (!m_tex_fbo) glGenFramebuffers(1, &m_tex_fbo);

    GLenum attachment = downloadAttachmentType(m_pixel_type);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_tex_fbo);
    glFramebufferTextureLayer(GL_READ_FRAMEBUFFER,
        attachment, m_tex, level, z);

    // Check if Texture and Framebuffer is valid to hacky read
    if (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &m_tex_fbo);
        m_tex_fbo = 0;
        return;
    }

    GLint read = 0;
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

void GLTexture::compatDownload2D(int x, int y, int w, int h, int level, void* data) {
    if (!m_tex_fbo) glGenFramebuffers(1, &m_tex_fbo);

    GLenum attachment = downloadAttachmentType(m_pixel_type);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_tex_fbo);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER,
        attachment, m_tex_target, m_tex, level);

    // Check if Texture and Framebuffer is valid to hacky read
    if (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &m_tex_fbo);
        m_tex_fbo = 0;
        return;
    }

    GLint read = 0;
    glGetIntegerv(GL_READ_BUFFER, &read);
    // Read Framebuffer Pixels
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(x, y, w, h,
        toValue(m_transfer_format),
        toValue(m_transfer_size),
        data);

    // Restore Read Buffer
    glReadBuffer(read);
}

void GLTexture::compatDownload1D(int x, int size, int level, void* data) {
    if (!m_tex_fbo) glGenFramebuffers(1, &m_tex_fbo);

    GLenum attachment = downloadAttachmentType(m_pixel_type);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_tex_fbo);
    glFramebufferTexture1D(GL_READ_FRAMEBUFFER,
        attachment, GL_TEXTURE_1D, m_tex, level);

    // Check if Texture and Framebuffer is valid to hacky read
    if (glCheckFramebufferStatus(GL_READ_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &m_tex_fbo);
        m_tex_fbo = 0;
        return;
    }

    GLint read = 0;
    glGetIntegerv(GL_READ_BUFFER, &read);
    // Read Framebuffer Pixels
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(x, 0, size, 1,
        toValue(m_transfer_format),
        toValue(m_transfer_size),
        data);

    // Restore Read Buffer
    glReadBuffer(read);
}

// --------------------------------
// OpenGL Texture: Clear Attachment
// --------------------------------

static GLenum clearAttachmentType(GPUTexturePixelType type) {
    switch (type) {
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT16:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT24:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT32:
            glClearDepth(0.0);
            return GL_DEPTH_ATTACHMENT;

        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH24_STENCIL8:
            glClearDepth(0.0);
            glClearStencil(0.0);
            return GL_DEPTH_STENCIL_ATTACHMENT;

        default: // RGBA Color Attachment
            glClearColor(0.0, 0.0, 0.0, 0.0);
            return GL_COLOR_ATTACHMENT0;
    }
}

static void clearAttachmentBuffer(GPUTexturePixelType type) {
    switch (type) {
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT16:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT24:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT32:
            glClear(GL_DEPTH_BUFFER_BIT);
            break;

        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH24_STENCIL8:
            glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            break;

        default: // RGBA Color Attachment
            glClear(GL_COLOR_BUFFER_BIT);
            break;
    }
}

// -----------------------------------
// OpenGL Texture: Clear Compatibility
// -----------------------------------

void GLTexture::compatClear3D(int x, int y, int z, int w, int h, int depth, int level) {
    if (!m_tex_fbo) glGenFramebuffers(1, &m_tex_fbo);

    GLenum attachment = clearAttachmentType(m_pixel_type);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_tex_fbo);
    glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER,
        attachment, m_tex, level, z);

    // Check if Texture and Framebuffer is valid to hacky clear
    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &m_tex_fbo);
        m_tex_fbo = 0;
        return;
    }

    // Locate Clear Region
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, w, h);
    // Clear Framebuffer Layers
    for (int i = 0; i < depth; i++) {
        glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0, m_tex, level, z + i);
        clearAttachmentBuffer(m_pixel_type);
    }
    // Restore Framebuffer
    glDisable(GL_SCISSOR_TEST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void GLTexture::compatClear2D(int x, int y, int w, int h, int level) {
    if (!m_tex_fbo) glGenFramebuffers(1, &m_tex_fbo);

    GLenum attachment = clearAttachmentType(m_pixel_type);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_tex_fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,
        attachment, m_tex_target, m_tex, level);

    // Check if Texture and Framebuffer is valid to hacky read
    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &m_tex_fbo);
        m_tex_fbo = 0;
        return;
    }

    // Clear Framebuffer Region
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, w, h);
    clearAttachmentBuffer(m_pixel_type);
    // Restore Framebuffer
    glDisable(GL_SCISSOR_TEST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void GLTexture::compatClear1D(int x, int size, int level) {
    if (!m_tex_fbo) glGenFramebuffers(1, &m_tex_fbo);

    GLenum attachment = clearAttachmentType(m_pixel_type);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_tex_fbo);
    glFramebufferTexture1D(GL_DRAW_FRAMEBUFFER,
        attachment, GL_TEXTURE_1D, m_tex, level);

    // Check if Texture and Framebuffer is valid to hacky read
    if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &m_tex_fbo);
        m_tex_fbo = 0;
        return;
    }

    // Clear Framebuffer Region
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, 0, size, 1);
    clearAttachmentBuffer(m_pixel_type);
    // Restore Framebuffer
    glDisable(GL_SCISSOR_TEST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
