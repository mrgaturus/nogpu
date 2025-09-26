// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "../private/texture.h"
#include "../private/context.h"
#include "../private/glad.h"
#include <climits>

// -----------------------
// Texture 1D: Constructor
// -----------------------

GLTexture1D::GLTexture1D(
    GLContext* ctx, GPUTexturePixelType type) : GLTexture(ctx) {
        m_pixel_type = type;
        m_transfer_format = computeTransferFormat(type);
        m_transfer_size = computeTransferSize(type);
        m_tex_target = GL_TEXTURE_1D;
}

// -------------------------------
// Texture 1D: Buffer Manipulation
// -------------------------------

void GLTexture1D::allocate(int size, int levels) {
    m_ctx->makeCurrentTexture(this);
    this->generateTexture();
    GLenum target = m_tex_target;
    // Allocate Texture Storage
    levels = levels_power_of_two(size, size, levels);
    glTexStorage1D(target, levels, toValue(m_pixel_type), size);

    // Set Texture Dimensions
    m_levels = levels;
    m_width = size;
    m_height = 1;
    m_depth = 1;
}

void GLTexture1D::upload(int x, int size, int level, void* data) {
    m_ctx->makeCurrentTexture(this);
    GLenum target = m_tex_target;
    // Upload Texture Data
    glBindTexture(target, m_tex);
    glTexSubImage1D(target, level, x, size,
        toValue(m_transfer_format),
        toValue(m_transfer_size), data);
}

void GLTexture1D::download(int x, int size, int level, void* data) {
    m_ctx->makeCurrentTexture(this);
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);

    // Use Optimized glGetTextureSubImage if available
    if (GLAD_GL_ARB_get_texture_sub_image) {
        glGetTextureSubImage(m_tex, level,
            x, 0, 0, size, 1, 1,
            toValue(m_transfer_format),
            toValue(m_transfer_size),
            INT_MAX, data);
    // Use Optimized glGetTexImage when full image
    } else if (x == 0 && size == m_width) {
        glGetTexImage(target, level,
            toValue(m_transfer_format),
            toValue(m_transfer_size),
            data);
    // Use Framebuffer Trick for Old Devices
    } else compatDownload1D(x, size, level, data);
}

void GLTexture1D::clear(int x, int size, int level) {
    m_ctx->makeCurrentTexture(this);
    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);

    // Use Optimized glClearTexSubImage if available
    if (GLAD_GL_ARB_clear_texture) {
        glClearTexSubImage(m_tex, level,
            x, 0, 0, size, 1, 1,
            toValue(m_transfer_format),
            toValue(m_transfer_size),
            nullptr);
    // Use Framebuffer Trick for Old Devices
    } else compatClear1D(x, size, level);
}

// -----------------------------------------
// Texture 1D: Buffer Manipulation using PBO
// -----------------------------------------

void GLTexture1D::unpack(int x, int size, int level, GPUBuffer *pbo, int offset) {
    m_ctx->makeCurrentTexture(this);

    // Copy PBO Pixels to Texture
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf->m_vbo);
    this->upload(x, size, level, reinterpret_cast<void*>(offset));
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    this->generateSync();
}

void GLTexture1D::pack(int x, int size, int level, GPUBuffer *pbo, int offset) {
    m_ctx->makeCurrentTexture(this);

    // Copy Texture Pixels to PBO
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, buf->m_vbo);
    this->download(x, size, level, reinterpret_cast<void*>(offset));
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    this->generateSync();
}
