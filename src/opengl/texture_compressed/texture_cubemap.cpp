// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "../nogpu/opengl_compressed.h"
#include "../nogpu/opengl_context.h"
#include "../glad/glad.h"

// -------------------------------
// Texture 2D Cubemap: Constructor
// -------------------------------

GLCompressedCubemap::GLCompressedCubemap(GLContext* ctx, GPUTextureCompressedType type) : GLTexture(ctx) {
        m_pixel_type = GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED;
        m_pixel_format = GPUTexturePixelFormat::TEXTURE_FORMAT_COMPRESSED;
        m_tex_target = GL_TEXTURE_CUBE_MAP;
        m_compressed_type = type;
}

void GLCompressedCubemap::allocate(int w, int h, int levels) {
    m_ctx->gl__makeCurrent();

    this->generateTexture();
    GLenum target = m_tex_target;
    glTexStorage2D(target, levels, toValue(m_pixel_type), w, h);

    // Check Allocation Error
    GLenum error = glGetError();
    switch (error) {
        case GL_INVALID_ENUM:
            GPULogger::error("invalid pixel type for Cubemap %p", this);
        case GL_INVALID_OPERATION:
            GPULogger::error("invalid levels count for Cubemap %p", this);
        case GL_INVALID_VALUE:
            GPULogger::error("invalid size for Cubemap %p", this);
    }

    // Check Texture Errors
    if (error != GL_NO_ERROR)
        return;

    // Set Texture Dimensions
    m_width = w;
    m_height = h;
    m_depth = 1;
}

// ------------------------------------
// Texture Cubemap: Buffer Manipulation
// ------------------------------------

void GLCompressedCubemap::upload(GPUTextureCubemapSide side, int x, int y, int w, int h, int level, void* data, int bytes) {
    m_ctx->gl__makeCurrent();

    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glCompressedTexSubImage2D(toValue(side), level, x, y, w, h,
        toValue(m_compressed_type), bytes, data);

    // Check Uploading Error
    GLenum error = glGetError();
    switch (error) {
        case GL_INVALID_OPERATION:
            GPULogger::error("failed uploading pixels for Cubemap %p", this);
        case GL_INVALID_VALUE:
            GPULogger::error("failed uploading parameters for Cubemap %p", this);
        case GL_INVALID_ENUM:
            GPULogger::error("invalid pixel format/type for Cubemap %p", this);
    }
}

// ----------------------------------------------
// Texture Cubemap: Buffer Manipulation using PBO
// ----------------------------------------------

void GLCompressedCubemap::unpack(GPUTextureCubemapSide side, int x, int y, int w, int h, int level, GPUBuffer *pbo, int bytes, int offset) {
    m_ctx->gl__makeCurrent();

    // Copy PBO Pixels to Texture
    GLBuffer* buf = static_cast<GLBuffer*>(pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buf->m_vbo);
    this->upload(side, x, y, w, h, level, reinterpret_cast<void*>(offset), bytes);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
