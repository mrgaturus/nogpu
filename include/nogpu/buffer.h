// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_BUFFER_H
#define NOGPU_BUFFER_H

// -------------------
// GPU Objects: Buffer
// -------------------

enum class GPUBufferUsage : int {
    BUFFER_USAGE_STREAM_DRAW,
    BUFFER_USAGE_STREAM_READ,
    BUFFER_USAGE_STREAM_COPY,
    // Static Buffer Usage
    BUFFER_USAGE_STATIC_DRAW,
    BUFFER_USAGE_STATIC_READ,
    BUFFER_USAGE_STATIC_COPY,
    // Dynamic Buffer Usage
    BUFFER_USAGE_DYNAMIC_DRAW,
    BUFFER_USAGE_DYNAMIC_READ,
    BUFFER_USAGE_DYNAMIC_COPY
};

enum class GPUBufferMapping : int {
    BUFFER_MAP_READ_BIT = 1 << 0,
    BUFFER_MAP_WRITE_BIT = 1 << 1,
    BUFFER_MAP_INVALIDATE_RANGE_BIT = 1 << 2,
    BUFFER_MAP_INVALIDATE_BUFFER_BIT = 1 << 3,
    BUFFER_MAP_FLUSH_EXPLICIT_BIT = 1 << 4,
    BUFFER_MAP_UNSYNCHRONIZED_BIT = 1 << 5
};

class GPUBuffer {
    protected: int m_bytes;
    public: virtual void destroy() = 0;
    public: int getBytes() { return m_bytes; };

    public: // GPU Buffer Usage
        virtual void orphan(int bytes, GPUBufferUsage usage) = 0;
        virtual void upload(void *data, int bytes, GPUBufferUsage usage) = 0;
        virtual void update(void *data, int bytes, int offset) = 0;
        virtual void download(void *data, int bytes, int offset) = 0;
        virtual void copy(GPUBuffer *dest, int bytes, int offset_read, int offset_write) = 0;
        virtual void clear(int offset, int bytes) = 0;
    public: // GPU Buffer Usage: Mapping
        virtual void* map(int bytes, int offset, GPUBufferMapping flags) = 0;
        virtual void unmap() = 0;

    public: // GPU Buffer Fences
        virtual void syncEnable(bool value) = 0;
        virtual void syncCPU() = 0;
        virtual void syncGPU() = 0;
};

// -------------------------
// GPU Objects: Vertex Array
// -------------------------

enum class GPUAttributeSize : int {
    ATTRIBUTE_SIZE_1,
    ATTRIBUTE_SIZE_2,
    ATTRIBUTE_SIZE_3,
    ATTRIBUTE_SIZE_4
};

enum class GPUAttributeType : int {
    ATTRIBUTE_TYPE_BYTE,
    ATTRIBUTE_TYPE_SHORT,
    ATTRIBUTE_TYPE_INT,
    ATTRIBUTE_TYPE_UNSIGNED_BYTE,
    ATTRIBUTE_TYPE_UNSIGNED_SHORT,
    ATTRIBUTE_TYPE_UNSIGNED_INT,
    ATTRIBUTE_TYPE_FLOAT,
    ATTRIBUTE_TYPE_DOUBLE,
};

class GPUVertexArray {
    protected:
        GPUBuffer* m_array_buffer;
        GPUBuffer* m_elements_buffer;
    public: virtual void destroy() = 0;

    public: // GPU Vertex Array: Register
        virtual void useArrayBuffer(GPUBuffer* buffer) = 0;
        virtual void useElementsBuffer(GPUBuffer* buffer) = 0;
        virtual void defineAttribute(int index, GPUAttributeSize size, GPUAttributeType type, int stride, int offset) = 0;
        virtual void defineNormalized(int index, GPUAttributeSize size, GPUAttributeType type, int stride, int offset) = 0;
        virtual void defineInstanceDivisor(int index, int divisor) = 0;
        virtual void disableAttribute(int index) = 0;
        virtual void enableAttribute(int index) = 0;

    public: // GPU Vertex Array: Getters
        GPUBuffer* getArrayBuffer() { return m_array_buffer; }
        GPUBuffer* getElementsBuffer() { return m_elements_buffer; }
};

GPUBufferMapping operator|(GPUBufferMapping a, GPUBufferMapping b);
GPUBufferMapping operator&(GPUBufferMapping a, GPUBufferMapping b);
#endif // NOGPU_BUFFER_H
