// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/texture.h"
#include "private/context.h"
#include "private/glad.h"

GLTexture::GLTexture(GLContext* ctx) {
    ctx->gl__makeCurrent();

    // Generate OpenGL Texture
    glGenTextures(1, &m_tex);
    m_sync_check = true;
    m_sync = nullptr;
    m_tex_fbo = 0;
    m_ctx = ctx;
}

void GLTexture::destroy() {
    m_ctx->gl__makeCurrent();

    // Destroy OpenGL Texture
    if (m_sync) glDeleteSync(m_sync);
    if (m_tex_fbo) glDeleteFramebuffers(1, &m_tex_fbo);
    glDeleteTextures(1, &m_tex);
    m_sync = nullptr;
    m_ctx = nullptr;

    // Dealloc Object
    delete this;
}

// -------------------------
// GPU Objects: Texture Base
// -------------------------

void GLTexture::setTransferSize(GPUTextureTransferSize size) {
    m_ctx->gl__makeCurrent();
    // Check if Pixel Type has transfer mode not fixed
    if (!canTransferChange(m_pixel_type)) {
        GPUReport::error("transfer size cannot be changed for %p", this);
        return;
    }
    
    // Change Transfer Size
    m_transfer_size = size;
}

void GLTexture::setTransferFormat(GPUTextureTransferFormat format) {
    m_ctx->gl__makeCurrent();
    // Check if Pixel Type has transfer mode not fixed
    if (!canTransferChange(m_pixel_type)) {
        GPUReport::error("transfer format cannot be changed for %p", this);
        return;
    }

    // Change Transfer Format
    m_transfer_format = format;
}

void GLTexture::setSwizzle(GPUTextureSwizzle swizzle) {
    m_ctx->gl__makeCurrent();
    m_swizzle = swizzle;

    // Change Texture Swizzle
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_R, toValue(swizzle.r));
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_G, toValue(swizzle.g));
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_B, toValue(swizzle.b));
    glTexParameteri(target, GL_TEXTURE_SWIZZLE_A, toValue(swizzle.a));

    if (glGetError() != GL_NO_ERROR)
        GPUReport::error("failed set texture swizzle for %p", this);
}

void GLTexture::setFilter(GPUTextureFilter filter) {
    m_ctx->gl__makeCurrent();
    m_filter = filter;

    // Change Texture Filter
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, toValue(filter.minify));
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, toValue(filter.magnify));

    if (glGetError() != GL_NO_ERROR)
        GPUReport::error("failed set texture filtering for %p", this);
}

void GLTexture::setWrap(GPUTextureWrap wrap) {
    m_ctx->gl__makeCurrent();
    m_wrap = wrap;

    // Change Texture Wrapping
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, toValue(wrap.s));
    glTexParameteri(target, GL_TEXTURE_WRAP_T, toValue(wrap.t));
    glTexParameteri(target, GL_TEXTURE_WRAP_R, toValue(wrap.r));

    if (glGetError() != GL_NO_ERROR)
        GPUReport::error("failed set texture wrapping for %p", this);
}

// ---------------------------------
// GPU Objects: Texture Base Mipmaps
// ---------------------------------

void GLTexture::generateTexture() {
    m_ctx->gl__makeCurrent();
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
    m_ctx->gl__makeCurrent();

    // Generate Texture Mipmaps
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glGenerateMipmap(target);

    if (glGetError() != GL_NO_ERROR)
        GPUReport::error("failed generate texture mipmaps for %p", this);
}

// -------------------------
// GPU Objects: Texture Sync
// -------------------------

void GLTexture::syncCPU() {
    m_ctx->gl__makeCurrent();
    // Stall CPU until Fence Signaled
    if (m_sync_check && m_sync)
        glClientWaitSync(m_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
}

void GLTexture::syncGPU() {
    m_ctx->gl__makeCurrent();
    // Stall GL Queue until Fence Signaled
    if (m_sync_check && m_sync)
        glWaitSync(m_sync, 0, GL_TIMEOUT_IGNORED);
}

void GLTexture::syncEnable() {
    m_ctx->gl__makeCurrent();
    m_sync_check = true;
}

void GLTexture::syncDisable() {
    m_ctx->gl__makeCurrent();
    m_sync_check = false;

    // Remove Sync Object
    if (m_sync) {
        glDeleteSync(m_sync);
        m_sync = nullptr;
    }
}

// -------------------------------
// GPU Objects: Texture Sync Stamp
// -------------------------------

void GLTexture::generateSync() {
    if (!m_sync_check) return;
    m_ctx->gl__makeCurrent();

    // Stamp Sync Object
    if (m_sync) glDeleteSync(m_sync);
    m_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}
