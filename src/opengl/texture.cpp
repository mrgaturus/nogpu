// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/texture.h"
#include "private/context.h"
#include "private/glad.h"

GLTexture::GLTexture(GLContext* ctx) {
    ctx->makeCurrentTexture(this);

    // Generate OpenGL Texture
    glGenTextures(1, &m_tex);
    m_tex_fbo = 0;
    m_ctx = ctx;
}

void GLTexture::destroy() {
    m_ctx->makeCurrentTexture(this);

    // Destroy OpenGL Texture
    if (m_tex_fbo) glDeleteFramebuffers(1, &m_tex_fbo);
    if (m_tex) glDeleteTextures(1, &m_tex);
    m_ctx = nullptr;
    // Dealloc Object
    delete this;
}

// -------------------------
// GPU Objects: Texture Base
// -------------------------

void GLTexture::setTransferSize(GPUTextureTransferSize size) {
    m_ctx->makeCurrentTexture(this);
    // Check if Pixel Type has transfer mode not fixed
    if (!canTransferChange(m_pixel_type)) {
        GPUReport::error("transfer size cannot be changed");
        return;
    }
    
    // Change Transfer Size
    m_transfer_size = size;
}

void GLTexture::setTransferFormat(GPUTextureTransferFormat format) {
    m_ctx->makeCurrentTexture(this);
    // Check if Pixel Type has transfer mode not fixed
    if (!canTransferChange(m_pixel_type)) {
        GPUReport::error("transfer format cannot be changed");
        return;
    }

    // Change Transfer Format
    m_transfer_format = format;
}

void GLTexture::setSwizzle(GPUTextureSwizzle swizzle) {
    m_ctx->makeCurrentTexture(this);
    m_swizzle = swizzle;

    // Change Texture Swizzle
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, toValue(swizzle.r));
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, toValue(swizzle.g));
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, toValue(swizzle.b));
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, toValue(swizzle.a));
}

void GLTexture::setFilter(GPUTextureFilter filter) {
    m_ctx->makeCurrentTexture(this);
    m_filter = filter;

    // Change Texture Filter
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, toValue(filter.minify));
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, toValue(filter.magnify));
}

void GLTexture::setWrap(GPUTextureWrap wrap) {
    m_ctx->makeCurrentTexture(this);
    m_wrap = wrap;

    // Change Texture Wrapping
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, toValue(wrap.s));
    glTexParameteri(target, GL_TEXTURE_WRAP_T, toValue(wrap.t));
    glTexParameteri(target, GL_TEXTURE_WRAP_R, toValue(wrap.r));
}

// ---------------------------------
// GPU Objects: Texture Base Mipmaps
// ---------------------------------

void GLTexture::generateTexture() {
    m_ctx->makeCurrentTexture(this);
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);

    // Reallocate if Allocated
    GLint checkAllocated = GL_FALSE;
    glGetTexParameteriv(target, GL_TEXTURE_IMMUTABLE_FORMAT, &checkAllocated);
    if (checkAllocated != GL_FALSE) {
        glDeleteTextures(1, &m_tex);
        glGenTextures(1, &m_tex);
        glBindTexture(target, m_tex);
    }
}

void GLTexture::generateMipmaps() {
    m_ctx->makeCurrentTexture(this);

    // Generate Texture Mipmaps
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glGenerateMipmap(target);
}

// -------------------------
// GPU Objects: Texture Sync
// -------------------------

GPUFence* GLTexture::syncFence() {
    m_ctx->makeCurrentTexture(this);
    return m_ctx->syncFence();
}
