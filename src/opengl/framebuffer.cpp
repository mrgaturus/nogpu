// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/framebuffer.h"
#include "private/context.h"
#include "private/glad.h"
#include <stdlib.h>
#include <string.h>

GLFrameBuffer::GLFrameBuffer(GLContext* ctx) {
    ctx->makeCurrentTexture(this);
    glGenFramebuffers(1, &m_fbo);
    // Initialize Current List
    m_colors_index.indexes = &m_color_index;
    m_colors_index.links = &m_color;
    m_colors_index.capacity = 1;
    m_colors_index.count = 0;
    m_ctx = ctx;
}

void GLFrameBuffer::destroy() {
    m_ctx->makeCurrentTexture(this);
    // Destroy FrameBuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &m_fbo);
    // Destroy Color Indexes
    if (m_colors_index.capacity > 1)
        free(m_colors_index.links);

    // Destroy Object
    delete this;
}

// -------------------------------
// Framebuffer Attachment: Indexes
// -------------------------------

void GLFrameBuffer::reserveIndexes(int count) {
    GLRenderIndexes* list = &m_colors_index;
    // Reserve New Buffer when Exceeded
    list->count = count;
    if (count > list->capacity) {
        free(list->links);
        // Allocate New Indexes Buffer
        int bytes_index = count * sizeof(GLuint);
        int bytes_list = count * sizeof(GLRenderLink*);
        char* buffer = (char*) malloc(bytes_list + bytes_index);
        list->links = reinterpret_cast<GLRenderLink**>(buffer);
        list->indexes = reinterpret_cast<GLuint*>(buffer + bytes_list);
    }
}

void GLFrameBuffer::updateIndexes() {
    GLRenderIndexes* list = &m_colors_index;
    // Update Color Links
    const int count = list->count;
    for (int i = 0; i < count; i++) {
        int idx = list->indexes[i];
        GLRenderLink* link = m_colors.get_key(idx);
        // Update Link Pointer and Invalidate
        list->links[idx] = link;
        if (link != nullptr)
            link->tex_cache = 0;
    }

    // Update Current Color
    if (count > 0) {
        m_color = list->links[0];
        m_color_index = list->indexes[0];
    } else {
        m_color = nullptr;
        m_color_index = 0;
    }
}

// ------------------------------
// Framebuffer Attachment: Attach
// ------------------------------

void GLFrameBuffer::attachColor(GPURenderBuffer *target, int index) {
    m_ctx->makeCurrentTexture(this);
    if (index < 0) index = 0;
    if (target == nullptr) {
        GPUReport::error("invalid renderbuffer");
        return;
    }

    // Check Attachment Buffer
    auto buffer = dynamic_cast<GLRenderBuffer*>(target);
    if (!canTransferChange(buffer->m_pixel_type)) {
        GPUReport::error("renderbuffer is not color pixel type");
        return;
    }

    // Create Link Attachment
    GLRenderLink link{};
    link.target = buffer;
    link.tex_index = index;
    link.tex_cache = 0;
    // Update Render Link Pointer
    if (!m_colors.replace_key(index, link))
        this->updateIndexes();
}

void GLFrameBuffer::attachDepth(GPURenderBuffer *target) {
    auto buffer = dynamic_cast<GLRenderBuffer*>(target);
    m_ctx->makeCurrentTexture(this);

    // Check Attachment Buffer
    if (buffer == nullptr) {
        GPUReport::error("invalid renderbuffer");
        return;
    } else if (canTransferChange(buffer->m_pixel_type)) {
        GPUReport::error("renderbuffer is not depth pixel type");
        return;
    }

    // Change Depth Attachment
    m_depth.target = buffer;
    m_depth.slice.layer = 0;
    m_depth.slice.level = 0;
    m_depth.tex_index = 0;
}

void GLFrameBuffer::attachStencil(GPURenderBuffer *target) {
    auto buffer = dynamic_cast<GLRenderBuffer*>(target);
    m_ctx->makeCurrentTexture(this);

    // Check Attachment Buffer
    if (buffer == nullptr) {
        GPUReport::error("invalid render buffer");
        return;
    } else if (buffer->m_pixel_type != GPUTexturePixelType::TEXTURE_PIXEL_DEPTH24_STENCIL8) {
        GPUReport::error("renderbuffer is not stencil pixel type");
        return;
    }

    // Change Stencil Attachment
    m_stencil.target = buffer;
    m_stencil.slice.layer = 0;
    m_stencil.slice.level = 0;
    m_stencil.tex_index = 0;
}

// ------------------------------
// Framebuffer Attachment: Detach
// ------------------------------

void GLFrameBuffer::detachColor(int index) {
    m_ctx->makeCurrentTexture(this);
    if (index < 0) index = 0;
    // Update Render Link Pointer
    if (m_colors.remove_key(index))
        this->updateIndexes();
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
    m_ctx->makeCurrentTexture(this);
    // Set One Color Index
    this->reserveIndexes(1);
    this->m_colors_index.indexes[0] = index;
    this->updateIndexes();
}

void GLFrameBuffer::setColorIndexes(int *list, int count) {
    m_ctx->makeCurrentTexture(this);
    // Set Multiple Color Index
    this->reserveIndexes(count);
    void* indexes = m_colors_index.indexes;
    memcpy(indexes, list, count * sizeof(GLuint));
    this->updateIndexes();
}

// --------------------------------
// Framebuffer Usage: Buffer Slices
// --------------------------------

void GLFrameBuffer::setColorSlice(int index, int layer, int level) {
    m_ctx->makeCurrentTexture(this);
    // Update Color Indexes
    GLRenderLink* result = m_colors.get_key(index);
    if (result != nullptr) {
        result->slice.layer = layer;
        result->slice.level = level;
        this->updateIndexes();
    } else {
        GPUReport::warning("color attachment %d not found", index);
    }
}

void GLFrameBuffer::setDepthSlice(int layer, int level) {
    m_ctx->makeCurrentTexture(this);
    if (m_depth.target == nullptr) {
        GPUReport::warning("depth attachment not found");
        return;
    }

    // Change Slice and Clear Cache
    m_depth.slice.layer = layer;
    m_depth.slice.level = level;
    m_depth.tex_cache = 0;
}

void GLFrameBuffer::setStencilSlice(int layer, int level) {
    m_ctx->makeCurrentTexture(this);
    if (m_stencil.target == nullptr) {
        GPUReport::warning("stencil attachment not found");
        return;
    }

    // Change Slice and Clear Cache
    m_stencil.slice.layer = layer;
    m_stencil.slice.level = level;
    m_stencil.tex_cache = 0;
}

// ----------------------
// Framebuffer Attributes
// ----------------------

int GLFrameBuffer::getColorIndex() {
    m_ctx->makeCurrentTexture(this);

    // Return Current Color Index
    if (m_color == nullptr)
        GPUReport::warning("color attachment not found");
    return m_color_index;
}

int GLFrameBuffer::getColorIndexes(int *list, int capacity) {
    m_ctx->makeCurrentTexture(this);
    GLRenderIndexes* list0 = &m_colors_index;

    // Clamp Count to Capacity
    int count = list0->count;
    if (capacity > 0)
        count = (count <= capacity) ?
            count : capacity;

    // Output List to Pointer
    if (count > 0) {
        int* indexes = (int*) list0->indexes;
        memcpy(list, indexes, count * sizeof(int));
    }

    // Return Written Count
    return count;
}

GPURenderBuffer* GLFrameBuffer::getColorCurrent() {
    m_ctx->makeCurrentTexture(this);
    GPURenderBuffer* result = nullptr;
    GLRenderLink* color = m_color;

    // Return Current Attachment
    if (color != nullptr)
        result = color->target;
    return result;
}

GPURenderBuffer* GLFrameBuffer::getColor(int index) {
    m_ctx->makeCurrentTexture(this);
    GPURenderBuffer* result = nullptr;
    GLRenderLink* color = m_colors.get_key(index);

    // Return Current Attachment
    if (color != nullptr)
        result = color->target;
    return result;
}

GPURenderBuffer* GLFrameBuffer::getDepth() {
    m_ctx->makeCurrentTexture(this);
    return m_depth.target;
}

GPURenderBuffer* GLFrameBuffer::getStencil() {
    m_ctx->makeCurrentTexture(this);
    return m_stencil.target;
}

// -----------------------------
// Framebuffer Attributes: Slice
// -----------------------------

GPUFrameBufferSlice GLFrameBuffer::getColorSlice(int index) {
    m_ctx->makeCurrentTexture(this);
    GPURenderBuffer* result = nullptr;
    GLRenderLink* color = m_colors.get_key(index);

    // Return Color Slice
    if (color != nullptr)
        return color->slice;
    GPUReport::warning("color attachment %d not found", index);
    return GPUFrameBufferSlice{};
}

GPUFrameBufferSlice GLFrameBuffer::getDepthSlice() {
    m_ctx->makeCurrentTexture(this);
    // Return Depth Slice
    if (m_depth.target == nullptr)
        GPUReport::warning("depth attachment not found");
    return m_depth.slice;
}

GPUFrameBufferSlice GLFrameBuffer::getStencilSlice() {
    m_ctx->makeCurrentTexture(this);
    // Return Stencil Slice
    if (m_stencil.target == nullptr)
        GPUReport::warning("stencil attachment not found");
    return m_stencil.slice;
}
