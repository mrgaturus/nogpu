// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "private/framebuffer.h"
#include "private/context.h"


GLRenderBuffer::GLRenderBuffer(GLContext* ctx, GPUTexturePixelType type) {
    ctx->gl__makeCurrent();
}

void GLRenderBuffer::destroy() {
    m_ctx->gl__makeCurrent();
}

// --------------------------------
// OpenGL Renderbuffer Manipulation
// --------------------------------

void GLRenderBuffer::useTexture(GPUTexture* texture) {
    m_ctx->gl__makeCurrent();
}

void GLRenderBuffer::createTexture(int w, int h, int samples) {
    m_ctx->gl__makeCurrent();
}

void GLRenderBuffer::createTextureArray(int w, int h, int samples, int layers) {
    m_ctx->gl__makeCurrent();
}

void GLRenderBuffer::createOffscreen(int w, int h, int samples) {
    m_ctx->gl__makeCurrent();
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
