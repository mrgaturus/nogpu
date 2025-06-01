// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "opengl.h"

GLenum toValue(GPUBufferUsage usage) {
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
}

GLenum toValue(GPUBufferMapping flags) {
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
    ctx->gl__makeCurrent();
    m_ctx = ctx;

    // Create OpenGL Buffer
    glGenBuffers(1, &m_buffer_id);
    m_mapping = nullptr;
    m_sync = nullptr;
    m_bytes = 0;
}

void GLBuffer::destroy() {
    m_ctx->gl__makeCurrent();
    if (m_mapping) unmap();
    if (m_sync) glDeleteSync(m_sync);
    glDeleteBuffers(1, &m_buffer_id);

    // Dealloc Object
    delete this;
}

// ------------------------
// OpenGL GPU Buffer: Usage
// ------------------------

void GLBuffer::orphan(int bytes, GPUBufferUsage usage) {
    m_ctx->gl__makeCurrent();
    glBindBuffer(GL_COPY_WRITE_BUFFER, m_buffer_id);
    glBufferData(GL_COPY_WRITE_BUFFER, bytes, NULL, toValue(usage));

    if (glGetError() != GL_NO_ERROR)
        GPULogger::error("failed orphan buffer %p with %d bytes", this, bytes);
    else m_bytes = bytes;
};

void GLBuffer::upload(int bytes, void *data, GPUBufferUsage usage) {
    m_ctx->gl__makeCurrent();
    glBindBuffer(GL_COPY_WRITE_BUFFER, m_buffer_id);
    glBufferData(GL_COPY_WRITE_BUFFER, bytes, data, toValue(usage));

    if (glGetError() != GL_NO_ERROR)
        GPULogger::error("failed upload %d bytes to buffer %p", bytes, this);
    else m_bytes = bytes;
};

void GLBuffer::update(int bytes, int offset, void *data) {
    m_ctx->gl__makeCurrent();
    glBindBuffer(GL_COPY_WRITE_BUFFER, m_buffer_id);
    glBufferSubData(GL_COPY_WRITE_BUFFER, offset, bytes, data);

    if (glGetError() != GL_NO_ERROR)
        GPULogger::error("failed update %d:%d bytes to buffer %p", bytes, offset, this);
};

void GLBuffer::download(int bytes, int offset, void *data) {
    m_ctx->gl__makeCurrent();
    glBindBuffer(GL_COPY_READ_BUFFER, m_buffer_id);
    glGetBufferSubData(GL_COPY_READ_BUFFER, offset, bytes, data);

    if (glGetError() != GL_NO_ERROR)
        GPULogger::error("failed download %d:%d bytes from buffer %p", bytes, offset, this);
};

void GLBuffer::copy(GPUBuffer *data, int bytes, int offset_read, int offset_write) {
    m_ctx->gl__makeCurrent();
    glBindBuffer(GL_COPY_WRITE_BUFFER, ((GLBuffer*) data)->m_buffer_id);
    glBindBuffer(GL_COPY_READ_BUFFER, this->m_buffer_id);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
        offset_read, offset_write, bytes);

    if (glGetError() != GL_NO_ERROR)
        GPULogger::error("failed copy %d->%d:%d bytes from buffer %p to buffer %p",
            offset_read, offset_write, bytes, this, data);
};

// --------------------------
// OpenGL GPU Buffer: Mapping
// --------------------------

void* GLBuffer::map(int bytes, int offset, GPUBufferMapping flags) {
    m_ctx->gl__makeCurrent();
    if (m_mapping) {
        GPULogger::error("buffer %p is already mapped", this);
        return nullptr;
    }

    GLenum flags0 = toValue(flags);
    glBindBuffer(GL_COPY_WRITE_BUFFER, m_buffer_id);
    void* map = glMapBufferRange(GL_COPY_WRITE_BUFFER, offset, bytes, flags0);
    if (glGetError() != GL_NO_ERROR)
        GPULogger::error("failed mapping %d:%d bytes from buffer %p",
            offset, bytes, this);

    // Create Sync Object when Unsynchronized
    if (flags0 & GL_MAP_UNSYNCHRONIZED_BIT) {
        if (m_sync) glDeleteSync(m_sync);
        m_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    // Return Mapping
    m_mapping = map;
    return map;
};

void GLBuffer::unmap() {
    m_ctx->gl__makeCurrent();
    if (!m_mapping) {
        GPULogger::error("buffer %p is not mapped", this);
        return;
    }

    glBindBuffer(GL_COPY_WRITE_BUFFER, m_buffer_id);
    glUnmapBuffer(GL_COPY_WRITE_BUFFER);
    if (glGetError() != GL_NO_ERROR)
        GPULogger::error("failed unmapping buffer %p", this);

    m_mapping = nullptr;
};

// -------------------------------
// OpenGL GPU Buffer: Mapping Sync
// -------------------------------

void GLBuffer::syncCPU() {
    m_ctx->gl__makeCurrent();
    if (m_sync) glClientWaitSync(m_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
};

void GLBuffer::syncGPU() {
    m_ctx->gl__makeCurrent();
    if (m_sync) glWaitSync(m_sync, 0, GL_TIMEOUT_IGNORED);
};
