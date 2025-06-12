// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "nogpu/opengl_buffer.h"
#include "nogpu/opengl_context.h"
#include "glad/glad.h"

static GLint toValue(GPUAttributeSize size) {
    return static_cast<int>(size) + 1;
}

static GLenum toValue(GPUAttributeType type) {
    switch (type) {
        case GPUAttributeType::ATTRIBUTE_TYPE_BYTE:
            return GL_BYTE;
        case GPUAttributeType::ATTRIBUTE_TYPE_SHORT:
            return GL_SHORT;
        case GPUAttributeType::ATTRIBUTE_TYPE_INT:
            return GL_INT;
        case GPUAttributeType::ATTRIBUTE_TYPE_UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        case GPUAttributeType::ATTRIBUTE_TYPE_UNSIGNED_SHORT:
            return GL_UNSIGNED_SHORT;
        case GPUAttributeType::ATTRIBUTE_TYPE_UNSIGNED_INT:
            return GL_UNSIGNED_INT;
        case GPUAttributeType::ATTRIBUTE_TYPE_FLOAT:
            return GL_FLOAT;
        case GPUAttributeType::ATTRIBUTE_TYPE_DOUBLE:
            return GL_DOUBLE;
    }

    // Reachable?
    return ~0;
}

// -----------------------------
// GPU Vertex Array: Constructor
// -----------------------------

GLVertexArray::GLVertexArray(GLContext* ctx) {
    ctx->gl__makeCurrent();
    m_ctx = ctx;

    // Generate Vertex Array
    glGenVertexArrays(1, &m_vao);
    m_array_buffer = nullptr;
    m_elements_buffer = nullptr;
}

void GLVertexArray::destroy() {
    m_ctx->gl__makeCurrent();
    glDeleteVertexArrays(1, &m_vao);
}

// -------------------------
// GPU Vertex Array: Buffers
// -------------------------

void GLVertexArray::useArrayBuffer(GPUBuffer* buffer) {
    m_ctx->gl__makeCurrent();

    if (buffer) {
        glBindVertexArray(m_vao);
        GLBuffer* buf = dynamic_cast<GLBuffer*>(buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buf->m_vbo);
    } else glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // Change Array Buffer
    m_array_buffer = buffer;
};

void GLVertexArray::useElementsBuffer(GPUBuffer* buffer) {
    m_ctx->gl__makeCurrent();

    if (buffer) {
        glBindVertexArray(m_vao);
        GLBuffer* buf = dynamic_cast<GLBuffer*>(buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->m_vbo);
    } else glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Change Elements Buffer
    m_elements_buffer = buffer;
};

// ----------------------------
// GPU Vertex Array: Attributes
// ----------------------------

void GLVertexArray::defineAttribute(int index, GPUAttributeSize size, GPUAttributeType type, int stride, int offset) {
    m_ctx->gl__makeCurrent();
    if (!m_array_buffer) {
        GPULogger::error("an array buffer not used to define a normalized attribute: #%d", index);
        return;
    }

    glBindVertexArray(m_vao);
    GLBuffer* buf = dynamic_cast<GLBuffer*>(m_array_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buf->m_vbo);
    glVertexAttribPointer(index, toValue(size), toValue(type),
        false, stride, reinterpret_cast<void*>(offset));

    if (glGetError() != GL_NO_ERROR) {
        GPULogger::error("failed define attribute #%d", index);
        return;
    }

    // Enable Attribute by Default
    glEnableVertexAttribArray(index);
};

void GLVertexArray::defineNormalized(int index, GPUAttributeSize size, GPUAttributeType type, int stride, int offset) {
    m_ctx->gl__makeCurrent();
    if (!m_array_buffer) {
        GPULogger::error("an array buffer not used to define a normalized attribute: #%d", index);
        return;
    } else if (type == GPUAttributeType::ATTRIBUTE_TYPE_FLOAT || type == GPUAttributeType::ATTRIBUTE_TYPE_DOUBLE) {
        GPULogger::error("float or double cannot be normalized for attribute: #%d", index);
        return;
    }

    glBindVertexArray(m_vao);
    GLBuffer* buf = dynamic_cast<GLBuffer*>(m_array_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buf->m_vbo);
    glVertexAttribPointer(index, toValue(size), toValue(type),
        true, stride, reinterpret_cast<void*>(offset));

    if (glGetError() != GL_NO_ERROR) {
        GPULogger::error("failed define normalized attribute: #%d", index);
        return;
    }

    // Enable Attribute by Default
    glEnableVertexAttribArray(index);
};

void GLVertexArray::disableAttribute(int index) {
    m_ctx->gl__makeCurrent();
    // Disable Vertex Attribute
    glBindVertexArray(m_vao);
    glDisableVertexAttribArray(index);

    if (glGetError() != GL_NO_ERROR)
        GPULogger::warning("attribute #%d is not defined", index);
};

void GLVertexArray::enableAttribute(int index) {
    m_ctx->gl__makeCurrent();
    // Enable Vertex Attribute
    glBindVertexArray(m_vao);
    glEnableVertexAttribArray(index);

    if (glGetError() != GL_NO_ERROR)
        GPULogger::warning("attribute #%d is not defined", index);
};
