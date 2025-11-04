// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "../private/compressed.h"
#include "../private/context.h"
#include "../private/glad.h"

// -------------------------------
// Texture 2D Cubemap: Constructor
// -------------------------------

GLCompressedCubemapArray::GLCompressedCubemapArray(GLContext* ctx, GPUTextureCompressedType type) : GLTexture(ctx) {
    m_pixel_type = GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED;
    m_transfer_format = GPUTextureTransferFormat::TEXTURE_FORMAT_COMPRESSED;
    m_transfer_size = GPUTextureTransferSize::TEXTURE_SIZE_COMPRESSED;
    m_tex_target = GL_TEXTURE_CUBE_MAP_ARRAY_ARB;
    m_compressed_type = type;

    // Check if Cubemap Array is Supported
    if (!GLAD_GL_ARB_texture_cube_map_array) {
        GPUReport::error("cubemap array is not supported");
        delete this;
        return;
    }
}

void GLCompressedCubemapArray::allocate(int w, int h, int layers, int levels) {
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

void GLCompressedCubemapArray::upload(GPUTextureCubemapSide side, int x, int y, int w, int h, int layer, int level, void* data, int bytes) {
    m_ctx->makeCurrentTexture(this);
    GLenum target = m_tex_target;
    // Upload Compressed Data
    glBindTexture(target, m_tex);
    glCompressedTexSubImage2D(toValue(side), level, x, y, w, h,
        toValue(m_compressed_type), bytes, data);
}

// ----------------------------------------------
// Texture Cubemap: Buffer Manipulation using PBO
// ----------------------------------------------

void GLCompressedCubemapArray::unpack(GPUTextureCubemapSide side, int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int bytes, int offset) {
    m_ctx->makeCurrentTexture(this);

    // Copy PBO Pixels to Texture
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf->m_vbo);
    this->upload(side, x, y, w, h, layer, level, reinterpret_cast<void*>(offset), bytes);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
