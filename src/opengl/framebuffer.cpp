// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "private/framebuffer.h"
#include "private/context.h"
#include "private/glad.h"

GLFrameBuffer::GLFrameBuffer(GLContext* ctx) {
    ctx->gl__makeCurrent();
}

void GLFrameBuffer::destroy() {
    m_ctx->gl__makeCurrent();
}

// -----------------------------
// OpenGL Framebuffer Attachment
// -----------------------------

void GLFrameBuffer::attachColor(GPURenderBuffer *target, int index) {
    m_ctx->gl__makeCurrent();

};

void GLFrameBuffer::attachStencil(GPURenderBuffer *target) {
    m_ctx->gl__makeCurrent();

};

void GLFrameBuffer::attachDepth(GPURenderBuffer *target) {
    m_ctx->gl__makeCurrent();

};

// -----------------------------
// OpenGL Framebuffer Detachment
// -----------------------------

void GLFrameBuffer::detachColor() {
    m_ctx->gl__makeCurrent();

}

void GLFrameBuffer::detachStencil() {
    m_ctx->gl__makeCurrent();

}

void GLFrameBuffer::detachDepth() {
    m_ctx->gl__makeCurrent();

}

// -----------------------------
// OpenGL Framebuffer Attributes
// -----------------------------

GPUFrameBufferStatus GLFrameBuffer::checkStatus() {
    m_ctx->gl__makeCurrent();
    return GPUFrameBufferStatus::FRAMEBUFFER_UNSUPPORTED;
};

GPURenderBuffer* GLFrameBuffer::getColor(int index) {
    m_ctx->gl__makeCurrent();
    return nullptr;
};

GPURenderBuffer* GLFrameBuffer::getStencil() {
    m_ctx->gl__makeCurrent();
    return nullptr;
};

GPURenderBuffer* GLFrameBuffer::getDepth() {
    m_ctx->gl__makeCurrent();
    return nullptr;
};
