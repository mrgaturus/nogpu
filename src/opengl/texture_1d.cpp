// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "nogpu/opengl_texture.h"
#include "nogpu/opengl_context.h"
#include "glad/glad.h"

// ------------------
// Texture 1D: Buffer
// ------------------

GLTextureBuffer::GLTextureBuffer(GLContext* ctx, GPUTexturePixelFormat format) {
    ctx->gl__makeCurrent();

    // Define Texture Buffer
    m_format = format;
    m_buffer = new GLBuffer(ctx);
    m_ctx = ctx;
}

void GLTextureBuffer::setFormat(GPUTexturePixelFormat format) {
    m_ctx->gl__makeCurrent();
    m_format = format;
}

GPUTexturePixelFormat GLTextureBuffer::getFormat() {
    m_ctx->gl__makeCurrent();
    return m_format;
}

GPUBuffer* GLTextureBuffer::getBuffer() {
    m_ctx->gl__makeCurrent();
    return m_buffer;
}

void GLTextureBuffer::destroy() {
    m_ctx->gl__makeCurrent();
    m_buffer->destroy();

    // Dealloc Object
    delete this;
}

// -----------------------
// Texture 1D: Constructor
// -----------------------

GLTexture1D::GLTexture1D(
    GLContext* ctx,
    GPUTexturePixelType type,
    GPUTexturePixelFormat format) : GLTexture(ctx) {
        m_pixel_type = type;
        m_pixel_format = format;
        m_tex_target = GL_TEXTURE_1D;
        
        // Check if texture 1D is supported
        if (!GPUDriver::checkFeature(GPUDriverFeature::DRIVER_TEXTURE_1D)) {
            GPULogger::error("texture 1d is not supported");
            delete this;
        }
}

// -------------------------------
// Texture 1D: Buffer Manipulation
// -------------------------------

void GLTexture1D::allocate(int size, int levels) {
    m_ctx->gl__makeCurrent();

    this->generateTexture();
    GLenum target = m_tex_target;
    glTexStorage1D(target, levels, toValue(m_pixel_type), size);

    // Check Allocation Error
    GLenum error = glGetError();
    switch (error) {
        case GL_INVALID_ENUM:
            GPULogger::error("invalid pixel type for %p", this); return;
        case GL_INVALID_OPERATION:
            GPULogger::error("invalid levels count for %p", this); return;
        case GL_INVALID_VALUE:
            GPULogger::error("invalid size for %p", this); return;
    }

    // Set Texture Dimensions
    m_w = size;
    m_h = 1;
}

void GLTexture1D::upload(int x, int size, int level, void* data) {
    m_ctx->gl__makeCurrent();

    GLenum target = m_tex_target;
    glBindTexture(target, m_tex);
    glTexSubImage1D(target, level, x, size,
        toValue(m_pixel_format), toValue(m_transfer_type), data);

    // Check Uploading Error
    GLenum error = glGetError();
    switch (error) {
        case GL_INVALID_OPERATION:
            GPULogger::error("failed uploading pixels for %p", this); return;
        case GL_INVALID_VALUE:
            GPULogger::error("failed uploading parameters for %p", this); return;
        case GL_INVALID_ENUM:
            GPULogger::error("invalid pixel format/type for %p", this); return;
    }
}

void GLTexture1D::download(int x, int size, int level, void* data) {
    m_ctx->gl__makeCurrent();
}

void GLTexture1D::unpack(int x, int size, int level, GPUBuffer *pbo, int offset) {
    m_ctx->gl__makeCurrent();
}

void GLTexture1D::pack(int x, int size, int level, GPUBuffer *pbo, int offset) {
    m_ctx->gl__makeCurrent();
}
