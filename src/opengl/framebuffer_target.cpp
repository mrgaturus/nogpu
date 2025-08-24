// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/framebuffer.h"
#include "private/context.h"
#include "private/glad.h"

GLRenderBuffer::GLRenderBuffer(GLContext* ctx, GPUTexturePixelType type) {
    ctx->gl__makeCurrent();
    if (type == GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED) {
        GPUReport::error("compressed framebuffer texture type is not supported");
        delete this;
        return;
    }

    // Define Renderbuffer initial attributes
    m_mode = GPURenderBufferMode::RENDERBUFFER_UNDEFINED;
    m_pixel_type = type;
    m_ctx = ctx;
}

void GLRenderBuffer::destroy() {
    m_ctx->gl__makeCurrent();

    // Destroy Object
    this->destroyInternal();
    delete this;
}

// -------------------------------------
// OpenGL Renderbuffer: Internal Texture
// -------------------------------------

void GLRenderBuffer::destroyInternal() {
    switch (m_mode) {
        // Destroy Offscreen Render Buffer
        case GPURenderBufferMode::RENDERBUFFER_UNDEFINED: break;
        case GPURenderBufferMode::RENDERBUFFER_OFFSCREEN:
            glDeleteRenderbuffers(1, m_object);
            break;

        // Destroy Texture Render Buffer
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_3D:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_ARRAY:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE_ARRAY:
            m_target->destroy();
            break;
        
        // Target Cannot be Destroyed
        case GPURenderBufferMode::RENDERBUFFER_TARGET:
        case GPURenderBufferMode::RENDERBUFFER_TARGET_3D:
        case GPURenderBufferMode::RENDERBUFFER_TARGET_ARRAY:
        case GPURenderBufferMode::RENDERBUFFER_TARGET_CUBEMAP:
        case GPURenderBufferMode::RENDERBUFFER_TARGET_CUBEMAP_ARRAY:
            break;
    }

    // Make RenderBuffer Undefined
    m_mode = GPURenderBufferMode::RENDERBUFFER_UNDEFINED;
    m_target = nullptr;
    m_object = nullptr;
    m_tex = 0;
    m_samples = 0;
    m_width = 0;
    m_height = 0;
}

void GLRenderBuffer::prepareInternal() {
    // Check Needs Re-create
    switch (m_mode) {
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_3D:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_ARRAY:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE_ARRAY:
            glDeleteTextures(1, m_object);
            break;
        
        default: // Create Texture
            this->destroyInternal();
            GLTexture* tex0 = new GLTexture(m_ctx);
            tex0->m_pixel_type = m_pixel_type;
            // Use Internal Texture
            m_target = tex0;
            m_object = &tex0->m_tex;
            break;
    }
}

// -------------------------------------
// OpenGL Renderbuffer: External Texture
// -------------------------------------

void GLRenderBuffer::updateExternal() {
    bool check = *(m_object) != m_tex;
    if (!check) return;

    // Check External Mode
    m_tex = *(m_object);
    m_samples = 1;
    switch (m_target->m_tex_target) {
        case GL_TEXTURE_1D:
        case GL_TEXTURE_2D:
        case GL_TEXTURE_RECTANGLE:
            m_mode = GPURenderBufferMode::RENDERBUFFER_TARGET;
            break;

        // Array Textures
        case GL_TEXTURE_3D:
            m_mode = GPURenderBufferMode::RENDERBUFFER_TARGET_3D;
            break;
        case GL_TEXTURE_1D_ARRAY:
        case GL_TEXTURE_2D_ARRAY:
            m_mode = GPURenderBufferMode::RENDERBUFFER_TARGET_ARRAY;
            break;

        // Cubemap Textures
        case GL_TEXTURE_CUBE_MAP:
            m_mode = GPURenderBufferMode::RENDERBUFFER_TARGET_CUBEMAP;
            break;
        case GL_TEXTURE_CUBE_MAP_ARRAY_ARB:
            m_mode = GPURenderBufferMode::RENDERBUFFER_TARGET_CUBEMAP_ARRAY;
            break;

        default: // Texture Not Supported
            m_mode = GPURenderBufferMode::RENDERBUFFER_UNDEFINED;
            GPUReport::error("unsupported external renderbuffer");
            break;
    }
}

void GLRenderBuffer::useTexture(GPUTexture* texture) {
    m_ctx->gl__makeCurrent();
    GLTexture* tex0 = dynamic_cast<GLTexture*>(texture);
    if (tex0->m_pixel_type != m_pixel_type) {
        GPUReport::error("mismatch texture pixel type for framebuffer");
        return;
    }

    // Destroy Previous
    this->destroyInternal();
    // Use External Texture
    m_target = tex0;
    m_object = &tex0->m_tex;
    this->updateExternal();
}

// -------------------------------------
// OpenGL Renderbuffer Attributes: Basic
// -------------------------------------

GPUTexturePixelType GLRenderBuffer::getPixelType() {
    m_ctx->gl__makeCurrent();
    this->updateExternal();
    return m_pixel_type;
}

GPURenderBufferMode GLRenderBuffer::getMode() {
    m_ctx->gl__makeCurrent();
    this->updateExternal();
    return m_mode;
}

// ------------------------------
// OpenGL Renderbuffer Attributes
// ------------------------------

GPUTexture* GLRenderBuffer::getTexture() {
    m_ctx->gl__makeCurrent();

    switch (m_mode) {
        case GPURenderBufferMode::RENDERBUFFER_UNDEFINED:
            GPUReport::error("cannot get texture of undefined renderbuffer");
            return nullptr;
        case GPURenderBufferMode::RENDERBUFFER_OFFSCREEN:
            GPUReport::error("cannot get texture of offscreen renderbuffer");
            return nullptr;

        default: // Return Texture
            return m_target;
    }
}

GPUTextureSize GLRenderBuffer::getSize() {
    m_ctx->gl__makeCurrent();
    GPUTextureSize size = {0, 0};

    switch (m_mode) {
        case GPURenderBufferMode::RENDERBUFFER_UNDEFINED:
            GPUReport::warning("cannot get dimensions of undefined renderbuffer");
            return size;
        case GPURenderBufferMode::RENDERBUFFER_OFFSCREEN:
            size.width = m_width;
            size.width = m_height;
            break;

        default: {
            GLTexture* target = m_target;
            size.width = target->m_width;
            size.height = target->m_height;
        }
    }

    // Return Dimensions
    return size;
}

int GLRenderBuffer::getWidth() {
    return this->getSize().width;
}

int GLRenderBuffer::getHeight() {
    return this->getSize().height;
}

int GLRenderBuffer::getDepth() {
    m_ctx->gl__makeCurrent();

    switch (m_mode) {
        case GPURenderBufferMode::RENDERBUFFER_UNDEFINED:
            GPUReport::warning("cannot get layers of undefined renderbuffer");
            return 0;
        case GPURenderBufferMode::RENDERBUFFER_OFFSCREEN:
            return 1;

        default: // Return Texture Layers
            return m_target->m_depth;
    }
}

int GLRenderBuffer::getLayers() {
    return getDepth();
}

int GLRenderBuffer::getLevels() {
    m_ctx->gl__makeCurrent();

    // Return Texture Levels
    switch (m_mode) {
        case GPURenderBufferMode::RENDERBUFFER_UNDEFINED:
            GPUReport::warning("cannot get levels of undefined renderbuffer");
            return 0;
        case GPURenderBufferMode::RENDERBUFFER_OFFSCREEN:
            return 1;

        default: // Texture levels
            return m_target->m_levels;        
    }
}

int GLRenderBuffer::getSamples() {
    m_ctx->gl__makeCurrent();
    if (m_mode == GPURenderBufferMode::RENDERBUFFER_UNDEFINED)
        GPUReport::warning("cannot get samples of undefined renderbuffer");

    // Return Texture Samples
    return m_samples;
}
