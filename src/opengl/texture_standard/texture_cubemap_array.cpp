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

GLTextureCubemapArray::GLTextureCubemapArray(
    GLContext* ctx, GPUTexturePixelType type) : GLTexture(ctx) {
        m_pixel_type = type;
        m_transfer_format = computeTransferFormat(type);
        m_transfer_size = computeTransferSize(type);
        m_tex_target = GL_TEXTURE_CUBE_MAP_ARRAY_ARB;

        // Check if Cubemap Array is Supported
        if (!GLAD_GL_ARB_texture_cube_map_array) {
            GPUReport::error("cubemap array is not supported");
            delete this;
            return;
        }
}

void GLTextureCubemapArray::allocate(int w, int h, int layers, int levels) {
    m_ctx->makeCurrentTexture(this);

    this->generateTexture();
    GLenum target = m_tex_target;
    // Allocate Texture Storage
    levels = levels_power_of_two(w, h, levels);
    glTexStorage3D(target, levels, toValue(m_pixel_type), w, h, layers);

    // Set Texture Dimensions
    m_levels = levels;
    m_width = w;
    m_height = h;
    m_depth = layers;
}

// ------------------------------------
// Texture Cubemap: Buffer Manipulation
// ------------------------------------

void GLTextureCubemapArray::upload(GPUTextureCubemapSide side, int x, int y, int w, int h, int layer, int level, void* data) {
    m_ctx->makeCurrentTexture(this);
    GLenum target = m_tex_target;
    // Upload Texture Data
    glBindTexture(target, m_tex);
    glTexSubImage2D(toValue(side), level, x, y, w, h,
        toValue(m_transfer_format),
        toValue(m_transfer_size), data);
}

void GLTextureCubemapArray::download(GPUTextureCubemapSide side, int x, int y, int w, int h, int layer, int level, void* data) {
    m_ctx->makeCurrentTexture(this);
    GLenum target = m_tex_target;
    GLenum target_side = toValue(side);
    GLint target_index = layer * 6 + (target_side - 0x8515);
    glBindTexture(target, m_tex);

    // Use Optimized glGetTextureSubImage if available
    if (GLAD_GL_ARB_get_texture_sub_image) {
        glGetTextureSubImage(m_tex, level,
            x, y, target_index, w, h, 1,
            toValue(m_transfer_format),
            toValue(m_transfer_size),
            INT_MAX, data);
    } else {
        m_tex_target = target_side;
        compatDownload3D(x, y, target_index, w, h, 1, level, data);
        m_tex_target = GL_TEXTURE_CUBE_MAP_ARRAY_ARB;
    }
}

void GLTextureCubemapArray::clear(GPUTextureCubemapSide side, int x, int y, int w, int h, int layer, int level) {
    m_ctx->makeCurrentTexture(this);
    GLenum target = m_tex_target;
    GLenum target_side = toValue(side);
    GLint target_index = layer * 6 + (target_side - 0x8515);
    glBindTexture(target, m_tex);

    // Use Optimized glClearTexSubImage if available
    if (GLAD_GL_ARB_clear_texture) {
        glClearTexSubImage(m_tex, level,
            x, y, target_index, w, h, 1,
            toValue(m_transfer_format),
            toValue(m_transfer_size),
            nullptr);
    // Use Framebuffer Trick for Old Devices
    } else {
        m_tex_target = target_side;
        compatClear3D(x, y, target_index, w, h, 1, level);
        m_tex_target = GL_TEXTURE_CUBE_MAP_ARRAY_ARB;
    }
}


// ----------------------------------------------
// Texture Cubemap: Buffer Manipulation using PBO
// ----------------------------------------------

void GLTextureCubemapArray::unpack(GPUTextureCubemapSide side, int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int offset) {
    m_ctx->makeCurrentTexture(this);

    // Copy PBO Pixels to Texture
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf->m_vbo);
    this->upload(side, x, y, w, h, layer, level, reinterpret_cast<void*>(offset));
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    this->generateSync();
}

void GLTextureCubemapArray::pack(GPUTextureCubemapSide side, int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int offset) {
    m_ctx->makeCurrentTexture(this);

    // Copy Texture Pixels to PBO
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, buf->m_vbo);
    this->download(side, x, y, w, h, layer, level, reinterpret_cast<void*>(offset));
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    this->generateSync();
}
