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

// ----------------------
// Framebuffer Attachment
// ----------------------

GPUFrameBufferStatus GLFrameBuffer::checkAttachments() {
    return GPUFrameBufferStatus::FRAMEBUFFER_UNSUPPORTED;
}

void GLFrameBuffer::attachColor(GPURenderBuffer *target, int index) {

}

void GLFrameBuffer::attachDepth(GPURenderBuffer *target) {

}

void GLFrameBuffer::attachStencil(GPURenderBuffer *target) {

}

void GLFrameBuffer::detachColor(int index) {

}

void GLFrameBuffer::detachDepth() {

}

void GLFrameBuffer::detachStencil() {

}

// -----------------
// Framebuffer Usage
// -----------------

void GLFrameBuffer::setColorNone() {

}

void GLFrameBuffer::setColorCurrent(int index) {

}

void GLFrameBuffer::setColorSlice(int layer, int level) {

}

void GLFrameBuffer::setDepthSlice(int layer, int level) {

}

void GLFrameBuffer::setStencilSlice(int layer, int level) {

}

// ----------------------
// Framebuffer Attributes
// ----------------------

int GLFrameBuffer::getColorCurrentIndex() {
    return 0;
}

bool GLFrameBuffer::getColorCurrentCheck() {
    return false;
}

GPURenderBuffer* GLFrameBuffer::getColorCurrent() {
    return nullptr;
}

GPURenderBuffer* GLFrameBuffer::getColor(int index) {
    return nullptr;
}

GPURenderBuffer* GLFrameBuffer::getDepth() {
    return nullptr;
}

GPURenderBuffer* GLFrameBuffer::getStencil() {
    return nullptr;
}
