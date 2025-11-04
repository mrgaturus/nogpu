// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "../private/compressed.h"
#include "../private/context.h"
#include "../private/glad.h"

// -----------------------
// Texture 1D: Constructor
// -----------------------

GLCompressed1D::GLCompressed1D(GLContext* ctx, GPUTextureCompressedType type) : GLTexture(ctx) {
    m_pixel_type = GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED;
    m_transfer_format = GPUTextureTransferFormat::TEXTURE_FORMAT_COMPRESSED;
    m_transfer_size = GPUTextureTransferSize::TEXTURE_SIZE_COMPRESSED;
    m_tex_target = GL_TEXTURE_1D;
    m_compressed_type = type;
}

// -------------------------------
// Texture 1D: Buffer Manipulation
// -------------------------------

void GLCompressed1D::allocate(int size, int levels) {
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

void GLCompressed1D::upload(int x, int size, int level, void* data, int bytes) {
    m_ctx->makeCurrentTexture(this);
    GLenum target = m_tex_target;
    // Upload Compressed Data
    glBindTexture(target, m_tex);
    glCompressedTexSubImage1D(target, level, x, size,
        toValue(m_compressed_type), bytes, data);
}

// -----------------------------------------
// Texture 1D: Buffer Manipulation using PBO
// -----------------------------------------

void GLCompressed1D::unpack(int x, int size, int level, GPUBuffer *pbo, int bytes, int offset) {
    m_ctx->makeCurrentTexture(this);

    // Copy PBO Pixels to Texture
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf->m_vbo);
    this->upload(x, size, level, reinterpret_cast<void*>(offset), bytes);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
