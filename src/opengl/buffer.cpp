// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/buffer.h"
#include "private/context.h"
#include "private/glad.h"

static GLenum toValue(GPUBufferUsage usage) {
    switch (usage) {
        // Stream Buffer Usage
        case GPUBufferUsage::BUFFER_USAGE_STREAM_DRAW:
            return GL_STREAM_DRAW;
        case GPUBufferUsage::BUFFER_USAGE_STREAM_READ:
            return GL_STREAM_READ;
        case GPUBufferUsage::BUFFER_USAGE_STREAM_COPY:
            return GL_STREAM_COPY;
        // Static Buffer Usage
        case GPUBufferUsage::BUFFER_USAGE_STATIC_DRAW:
            return GL_STATIC_DRAW;
        case GPUBufferUsage::BUFFER_USAGE_STATIC_READ:
            return GL_STATIC_READ;
        case GPUBufferUsage::BUFFER_USAGE_STATIC_COPY:
            return GL_STATIC_COPY;
        // Dynamic Buffer Usage
        case GPUBufferUsage::BUFFER_USAGE_DYNAMIC_DRAW:
            return GL_DYNAMIC_DRAW;
        case GPUBufferUsage::BUFFER_USAGE_DYNAMIC_READ:
            return GL_DYNAMIC_READ;
        case GPUBufferUsage::BUFFER_USAGE_DYNAMIC_COPY:
            return GL_DYNAMIC_COPY;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

static GLenum toValue(GPUBufferMapping flags) {
    GLenum result = 0;
    if ((int) (flags & GPUBufferMapping::BUFFER_MAP_READ_BIT))
        result |= GL_MAP_READ_BIT;
    if ((int) (flags & GPUBufferMapping::BUFFER_MAP_WRITE_BIT))
        result |= GL_MAP_WRITE_BIT;
    if ((int) (flags & GPUBufferMapping::BUFFER_MAP_INVALIDATE_RANGE_BIT))
        result |= GL_MAP_INVALIDATE_RANGE_BIT;
    if ((int) (flags & GPUBufferMapping::BUFFER_MAP_INVALIDATE_BUFFER_BIT))
        result |= GL_MAP_INVALIDATE_BUFFER_BIT;
    if ((int) (flags & GPUBufferMapping::BUFFER_MAP_FLUSH_EXPLICIT_BIT))
        result |= GL_MAP_FLUSH_EXPLICIT_BIT;
    if ((int) (flags & GPUBufferMapping::BUFFER_MAP_UNSYNCHRONIZED_BIT))
        result |= GL_MAP_UNSYNCHRONIZED_BIT;

    return result;
}

// ------------------------------
// OpenGL GPU Buffer: Constructor
// ------------------------------

GLBuffer::GLBuffer(GLContext* ctx) {
    ctx->makeCurrent(this);
    m_ctx = ctx;

    // Create OpenGL Buffer
    glGenBuffers(1, &m_vbo);
    m_mapping = nullptr;
    m_sync = nullptr;
    m_bytes = 0;
}

void GLBuffer::destroy() {
    m_ctx->makeCurrent(this);
    if (m_mapping) unmap();
    if (m_sync) glDeleteSync(m_sync);
    glDeleteBuffers(1, &m_vbo);

    // Dealloc Object
    delete this;
}

// ------------------------
// OpenGL GPU Buffer: Usage
// ------------------------

void GLBuffer::orphan(int bytes, GPUBufferUsage usage) {
    m_ctx->makeCurrent(this);
    glBindBuffer(GL_COPY_WRITE_BUFFER, m_vbo);
    glBufferData(GL_COPY_WRITE_BUFFER, bytes, NULL, toValue(usage));
    // Set Buffer Bytes
    m_bytes = bytes;
};

void GLBuffer::upload(int bytes, void *data, GPUBufferUsage usage) {
    m_ctx->makeCurrent(this);
    glBindBuffer(GL_COPY_WRITE_BUFFER, m_vbo);
    glBufferData(GL_COPY_WRITE_BUFFER, bytes, data, toValue(usage));
    // Set Buffer Bytes
    m_bytes = bytes;
};

void GLBuffer::update(int bytes, int offset, void *data) {
    m_ctx->makeCurrent(this);
    // Update Buffer Data
    glBindBuffer(GL_COPY_WRITE_BUFFER, m_vbo);
    glBufferSubData(GL_COPY_WRITE_BUFFER, offset, bytes, data);
};

void GLBuffer::download(int bytes, int offset, void *data) {
    m_ctx->makeCurrent(this);
    // Download Buffer Data
    glBindBuffer(GL_COPY_READ_BUFFER, m_vbo);
    glGetBufferSubData(GL_COPY_READ_BUFFER, offset, bytes, data);
};

void GLBuffer::copy(GPUBuffer *data, int bytes, int offset_read, int offset_write) {
    m_ctx->makeCurrent(this);
    GLBuffer* buf = static_cast<GLBuffer*>(data);
    // Copy Buffer Data from Other Buffer
    glBindBuffer(GL_COPY_WRITE_BUFFER, buf->m_vbo);
    glBindBuffer(GL_COPY_READ_BUFFER, this->m_vbo);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
        offset_read, offset_write, bytes);
};

// --------------------------
// OpenGL GPU Buffer: Mapping
// --------------------------

void* GLBuffer::map(int bytes, int offset, GPUBufferMapping flags) {
    m_ctx->makeCurrent(this);
    if (m_mapping) {
        GPUReport::error("buffer %p is already mapped", this);
        return nullptr;
    }

    GLenum flags0 = toValue(flags);
    glBindBuffer(GL_COPY_WRITE_BUFFER, m_vbo);
    void* map = glMapBufferRange(GL_COPY_WRITE_BUFFER, offset, bytes, flags0);
    // Create Sync Object when Unsynchronized
    if ((flags0 & GL_MAP_UNSYNCHRONIZED_BIT) && m_sync_check) {
        if (m_sync) glDeleteSync(m_sync);
        m_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    // Return Mapping
    m_mapping = map;
    return map;
};

void GLBuffer::unmap() {
    m_ctx->makeCurrent(this);
    if (!m_mapping) {
        GPUReport::error("buffer %p is not mapped", this);
        return;
    }

    // Remove Buffer Mapping
    glBindBuffer(GL_COPY_WRITE_BUFFER, m_vbo);
    glUnmapBuffer(GL_COPY_WRITE_BUFFER);
    m_mapping = nullptr;
};

// ------------------------------
// OpenGL GPU Buffer: Buffer Sync
// ------------------------------

void GLBuffer::syncEnable(bool value) {
    m_ctx->makeCurrent(this);
    m_sync_check = value;

    // Remove Sync Object
    if (value == false && m_sync) {
        glDeleteSync(m_sync);
        m_sync = nullptr;
    }
}

void GLBuffer::syncCPU() {
    m_ctx->makeCurrent(this);
    // Stall CPU until Fence Signaled
    if (m_sync_check && m_sync)
        glClientWaitSync(m_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
}

void GLBuffer::syncGPU() {
    m_ctx->makeCurrent(this);
    // Stall GL Queue until Fence Signaled
    if (m_sync_check && m_sync)
        glWaitSync(m_sync, 0, GL_TIMEOUT_IGNORED);
}
