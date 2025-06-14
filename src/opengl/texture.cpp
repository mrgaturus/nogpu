// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "nogpu/opengl_texture.h"
#include "nogpu/opengl_context.h"
#include "glad/glad.h"

GLTexture::GLTexture(GLContext* ctx) {
    ctx->gl__makeCurrent();

    // Generate OpenGL Texture
    glGenTextures(1, &m_tex);
    m_sync = nullptr;
    m_ctx = ctx;
}

void GLTexture::destroy() {
    m_ctx->gl__makeCurrent();

    // Destroy OpenGL Texture
    if (m_sync) glDeleteSync(m_sync);
    glDeleteTextures(1, &m_tex);
    m_sync = nullptr;
    m_ctx = nullptr;

    // Dealloc Object
    delete this;
}

// -------------------------
// GPU Objects: Texture Base
// -------------------------

void GLTexture::setTransferType(GPUTextureTransferType type) {
    m_ctx->gl__makeCurrent();
    m_transfer_type = type;
};

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
        GPULogger::error("failed set texture swizzle for %p", this);
};

void GLTexture::setFilter(GPUTextureFilter filter) {
    m_ctx->gl__makeCurrent();
    m_filter = filter;

    // Change Texture Filter
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, toValue(filter.minify));
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, toValue(filter.magnify));

    if (glGetError() != GL_NO_ERROR)
        GPULogger::error("failed set texture filtering for %p", this);
};

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
        GPULogger::error("failed set texture wrapping for %p", this);
};

// ---------------------------------
// GPU Objects: Texture Base Mipmaps
// ---------------------------------

void GLTexture::generateMipmaps() {
    m_ctx->gl__makeCurrent();

    // Generate Texture Mipmaps
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glGenerateMipmap(target);

    if (glGetError() != GL_NO_ERROR)
        GPULogger::error("failed generate texture mipmaps for %p", this);
};

// ------------------------------
// GPU Objects: Texture Base Sync
// ------------------------------

void GLTexture::syncCPU() {
    m_ctx->gl__makeCurrent();
    if (m_sync) glClientWaitSync(m_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
};

void GLTexture::syncGPU() {
    m_ctx->gl__makeCurrent();
    if (m_sync) glWaitSync(m_sync, 0, GL_TIMEOUT_IGNORED);
};
