// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "private/framebuffer.h"
#include "private/context.h"
#include "private/glad.h"
#include <stdlib.h>
#include <string.h>

GLFrameBuffer::GLFrameBuffer(GLContext* ctx) {
    ctx->gl__makeCurrent();
    glGenFramebuffers(1, &m_fbo);
    // Initialize Current List
    m_colors_index.list_index = &m_color_index;
    m_colors_index.list_link = &m_color;
    m_colors_index.capacity = 1;
    m_colors_index.count = 0;
}

void GLFrameBuffer::destroy() {
    m_ctx->gl__makeCurrent();
    // Destroy FrameBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &m_fbo);
    // Destroy Color Indexes
    if (m_colors_index.capacity > 0)
        free(m_colors_index.list_link);

    // Destroy Object
    delete this;
}

// -----------------------------------------
// Framebuffer Attachment: Check Attachments
// -----------------------------------------

GPUFrameBufferStatus GLFrameBuffer::checkAttachments() {
    m_ctx->gl__makeCurrent();
    return GPUFrameBufferStatus::FRAMEBUFFER_UNSUPPORTED;
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// ----------------------
// Framebuffer Attachment
// ----------------------

void GLFrameBuffer::attachColor(GPURenderBuffer *target, int index) {
    auto buffer = dynamic_cast<GLRenderBuffer*>(target);
    m_ctx->gl__makeCurrent();
    if (index < 0) index = 0;

    // Create Link Attachment
    GLRenderLink link{};
    link.target = buffer;
    link.tex_index = index;
    link.tex_cache = 0;
    // Update Render Link Pointer
    if (!m_colors.replace_key(index, link))
        m_color = m_colors.get_key(m_color_index);
}

void GLFrameBuffer::attachDepth(GPURenderBuffer *target) {
    auto buffer = dynamic_cast<GLRenderBuffer*>(target);
    m_ctx->gl__makeCurrent();
    // Change Depth Attachment
    m_depth.target = buffer;
    m_depth.tex_index = 0;
    m_depth.tex_cache = 0;
}

void GLFrameBuffer::attachStencil(GPURenderBuffer *target) {
    auto buffer = dynamic_cast<GLRenderBuffer*>(target);
    m_ctx->gl__makeCurrent();
    // Change Depth Attachment
    m_stencil.target = buffer;
    m_stencil.tex_index = 0;
    m_stencil.tex_cache = 0;
}

void GLFrameBuffer::detachColor(int index) {
    m_ctx->gl__makeCurrent();
    if (index < 0) index = 0;
    // Update Render Link Pointer
    if (m_colors.remove_key(index))
        m_color = m_colors.get_key(m_color_index);
}

void GLFrameBuffer::detachDepth() {
    this->attachDepth(nullptr);
}

void GLFrameBuffer::detachStencil() {
    this->attachStencil(nullptr);
}

// ------------------------------
// Framebuffer Usage: Color Index
// ------------------------------

void GLFrameBuffer::setColorIndex(int index) {
    m_ctx->gl__makeCurrent();
    if (m_colors_index.capacity > 1)
        free(m_colors_index.list_link);
    // Change Current List
    m_colors_index.list_index = &m_color_index;
    m_colors_index.list_link = &m_color;
    m_colors_index.capacity = 1;
    m_colors_index.count = 1;

    // Change Current Color
    m_color_index = index;
    m_color = m_colors.get_key(index);
    if (m_color != nullptr)
        m_color->tex_cache = 0;
}

void GLFrameBuffer::setColorIndexes(int *list, int count) {
    m_ctx->gl__makeCurrent();
}

// --------------------------------
// Framebuffer Usage: Buffer Slices
// --------------------------------

void GLFrameBuffer::setColorSlice(int layer, int level) {
    m_ctx->gl__makeCurrent();
    m_color_slice.layer = layer;
    m_color_slice.level = level;
    // Clear Color Cache
    if (m_color != nullptr)
        m_color->tex_cache = 0;
}

void GLFrameBuffer::setDepthSlice(int layer, int level) {
    m_ctx->gl__makeCurrent();
    // Change Slice and Clear Cache
    m_depth_slice.layer = layer;
    m_depth_slice.level = level;
    m_depth.tex_cache = 0;
}

void GLFrameBuffer::setStencilSlice(int layer, int level) {
    m_ctx->gl__makeCurrent();
    // Change Slice and Clear Cache
    m_stencil_slice.layer = layer;
    m_stencil_slice.level = level;
    m_stencil.tex_cache = 0;
}

// ----------------------
// Framebuffer Attributes
// ----------------------

int GLFrameBuffer::getColorIndex() {
    m_ctx->gl__makeCurrent();
    return m_color_index;
}

int GLFrameBuffer::getColorIndexes(int *list) {
    m_ctx->gl__makeCurrent();
    return m_color_index;
}

GPURenderBuffer* GLFrameBuffer::getColorCurrent() {
    m_ctx->gl__makeCurrent();
    GPURenderBuffer* result = nullptr;
    GLRenderLink* color = m_color;

    // Return Current Attachment
    if (color != nullptr)
        result = color->target;
    return result;
}

GPURenderBuffer* GLFrameBuffer::getColor(int index) {
    m_ctx->gl__makeCurrent();
    GPURenderBuffer* result = nullptr;
    GLRenderLink* color = m_colors.get_key(index);

    // Return Current Attachment
    if (color != nullptr)
        result = color->target;
    return result;
}

GPURenderBuffer* GLFrameBuffer::getDepth() {
    m_ctx->gl__makeCurrent();
    return m_depth.target;
}

GPURenderBuffer* GLFrameBuffer::getStencil() {
    m_ctx->gl__makeCurrent();
    return m_stencil.target;
}
