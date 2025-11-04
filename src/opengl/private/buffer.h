// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_BUFFER_H
#define OPENGL_BUFFER_H
#include <nogpu/buffer.h>

typedef int GLint;
typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef struct __GLsync *GLsync;

class GLContext;
class GLBuffer : public GPUBuffer {
    // GPU Buffer Usage
    void orphan(int bytes, GPUBufferUsage usage) override;
    void upload(void *data, int bytes, GPUBufferUsage usage) override;
    void update(void *data, int bytes, int offset) override;
    void download(void *data, int bytes, int offset) override;
    void copy(GPUBuffer *dest, int bytes, int offset_read, int offset_write) override;
    void clear(int offset, int bytes) override;
    // GPU Buffer Usage: Mapping
    GPUFence* syncFence() override;
    void* map(int bytes, int offset, GPUBufferMapping flags) override;
    void unmap() override;

    public: // GL Attributes
        GLContext* m_ctx;
        void* m_mapping;
        GLuint m_vbo;

    public: // Buffer Constructor
        friend GLContext;
        GLBuffer(GLContext* ctx);
        void destroy() override;
};

class GLVertexArray : GPUVertexArray {
    // GPU Vertex Array: Register
    void useArrayBuffer(GPUBuffer* buffer) override;
    void useElementsBuffer(GPUBuffer* buffer) override;
    void defineAttribute(int index, GPUAttributeSize size, GPUAttributeType type, int stride, int offset) override;
    void defineNormalized(int index, GPUAttributeSize size, GPUAttributeType type, int stride, int offset) override;
    void defineInstanceDivisor(int index, int divisor) override;
    void disableAttribute(int index) override;
    void enableAttribute(int index) override;

    public: // GL Attributes
        GLContext* m_ctx;
        GLuint m_vao;

    private: // Vertex Array Constructor
        friend GLContext;
        GLVertexArray(GLContext* ctx);
        void destroy() override;
};

#endif // OPENGL_BUFFER_H
