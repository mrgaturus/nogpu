// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "private/framebuffer.h"
#include "private/context.h"
#include "private/glad.h"

GLRenderBuffer::GLRenderBuffer(GLContext* ctx, GPUTexturePixelType type) {
    ctx->gl__makeCurrent();

    // Define RenderBuffer initial attributes
    m_mode = GPURenderBufferMode::RENDERBUFFER_UNDEFINED;
    m_pixel_type = type;
}

void GLRenderBuffer::destroyInternal() {
    switch (m_mode) {
        // Destroy Offscreen Render Buffer
        case GPURenderBufferMode::RENDERBUFFER_UNDEFINED: break;
        case GPURenderBufferMode::RENDERBUFFER_OFFSCREEN:
            glDeleteRenderbuffers(1, m_object);

        // Destroy Texture Render Buffer
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_ARRAY:
        case GPURenderBufferMode::RENDERBUFFER_TEXTURE_MULTISAMPLE:
            m_texture->destroy();
        
        // Target Cannot be Destroyed
        case GPURenderBufferMode::RENDERBUFFER_TARGET:
        case GPURenderBufferMode::RENDERBUFFER_TARGET_ARRAY:
        case GPURenderBufferMode::RENDERBUFFER_TARGET_CUBEMAP:
        case GPURenderBufferMode::RENDERBUFFER_TARGET_CUBEMAP_ARRAY:
            break;
    }

    // Make RenderBuffer Undefined
    m_mode = GPURenderBufferMode::RENDERBUFFER_UNDEFINED;
    m_object = nullptr;
}

void GLRenderBuffer::destroy() {
    m_ctx->gl__makeCurrent();

    // Destroy Object
    this->destroyInternal();
    delete this;
}

// --------------------------------
// OpenGL Renderbuffer Manipulation
// --------------------------------

void GLRenderBuffer::useTexture(GPUTexture* texture) {
    m_ctx->gl__makeCurrent();
    this->destroyInternal();
}

void GLRenderBuffer::createTexture(int w, int h, int samples) {
    m_ctx->gl__makeCurrent();
    this->destroyInternal();
}

void GLRenderBuffer::createTextureArray(int w, int h, int samples, int layers) {
    m_ctx->gl__makeCurrent();
    this->destroyInternal();
}

void GLRenderBuffer::createOffscreen(int w, int h, int samples) {
    m_ctx->gl__makeCurrent();
    this->destroyInternal();
}

// ------------------------------
// OpenGL Renderbuffer Attributes
// ------------------------------

GPUTexture* GLRenderBuffer::getTexture() {
    m_ctx->gl__makeCurrent();
    return nullptr;
}

GPUTextureSize GLRenderBuffer::getSize() {
    m_ctx->gl__makeCurrent();
    return (GPUTextureSize) {0, 0};
}

int GLRenderBuffer::getSamples() {
    m_ctx->gl__makeCurrent();
    return 0;
}

int GLRenderBuffer::getWidth() {
    m_ctx->gl__makeCurrent();
    return 0;
}

int GLRenderBuffer::getHeight() {
    m_ctx->gl__makeCurrent();
    return 0;
}

int GLRenderBuffer::getLayers() {
    m_ctx->gl__makeCurrent();
    return 0;
}
