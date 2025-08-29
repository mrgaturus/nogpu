// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "../private/texture.h"
#include "../private/context.h"
#include "../private/glad.h"
#include <climits>

// -------------------------------
// Texture 2D Cubemap: Constructor
// -------------------------------

GLTextureCubemap::GLTextureCubemap(
    GLContext* ctx, GPUTexturePixelType type) : GLTexture(ctx) {
        m_pixel_type = type;
        m_transfer_format = computeTransferFormat(type);
        m_transfer_size = computeTransferSize(type);
        m_tex_target = GL_TEXTURE_CUBE_MAP;
}

void GLTextureCubemap::allocate(int w, int h, int levels) {
    m_ctx->gl__makeCurrent();

    this->generateTexture();
    GLenum target = m_tex_target;
    // Allocate Texture Storage
    levels = levels_power_of_two(w, h, levels);
    glTexStorage2D(target, levels, toValue(m_pixel_type), w, h);

    // Set Texture Dimensions
    m_levels = levels;
    m_width = w;
    m_height = h;
    m_depth = 1;
}

// ------------------------------------
// Texture Cubemap: Buffer Manipulation
// ------------------------------------

void GLTextureCubemap::upload(GPUTextureCubemapSide side, int x, int y, int w, int h, int level, void* data) {
    m_ctx->gl__makeCurrent();
    GLenum target = m_tex_target;
    // Upload Texture Data
    glBindTexture(target, m_tex);
    glTexSubImage2D(toValue(side), level, x, y, w, h,
        toValue(m_transfer_format),
        toValue(m_transfer_size), data);
}

void GLTextureCubemap::download(GPUTextureCubemapSide side, int x, int y, int w, int h, int level, void* data) {
    m_ctx->gl__makeCurrent();
    GLenum target = m_tex_target;
    GLenum target_side = toValue(side);
    glBindTexture(target, m_tex);

    // Use Optimized glGetTextureSubImage if available
    if (GLAD_GL_ARB_get_texture_sub_image) {
        glGetTextureSubImage(m_tex, level,
            x, y, target_side - 0x8515, w, h, 1,
            toValue(m_transfer_format),
            toValue(m_transfer_size),
            INT_MAX, data);
    // Use Optimized glGetTexImage when full image
    } else if (x == 0 && y == 0 && w == m_width && h == m_height) {
        glGetTexImage(target_side, level,
            toValue(m_transfer_format),
            toValue(m_transfer_size),
            data);
    // Use Framebuffer Trick for Old Devices
    } else {
        m_tex_target = target_side;
        compatDownload2D(x, y, w, h, level, data);
        m_tex_target = GL_TEXTURE_CUBE_MAP;
    }
}

// ----------------------------------------------
// Texture Cubemap: Buffer Manipulation using PBO
// ----------------------------------------------

void GLTextureCubemap::unpack(GPUTextureCubemapSide side, int x, int y, int w, int h, int level, GPUBuffer *pbo, int offset) {
    m_ctx->gl__makeCurrent();

    // Copy PBO Pixels to Texture
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf->m_vbo);
    this->upload(side, x, y, w, h, level, reinterpret_cast<void*>(offset));
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    this->generateSync();
}

void GLTextureCubemap::pack(GPUTextureCubemapSide side, int x, int y, int w, int h, int level, GPUBuffer *pbo, int offset) {
    m_ctx->gl__makeCurrent();

    // Copy Texture Pixels to PBO
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, buf->m_vbo);
    this->download(side, x, y, w, h, level, reinterpret_cast<void*>(offset));
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    this->generateSync();
}
