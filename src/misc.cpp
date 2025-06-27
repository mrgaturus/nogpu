// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include <nogpu/buffer.h>
#include <nogpu/commands.h>

unsigned int next_power_of_two(unsigned int v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    // Portable for all C Compilers
    return v;
}

bool is_color(GPUTexturePixelType type) {
    switch (type) {
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT16:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT24:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT32:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH24_STENCIL8:
            return false;
        default: return true;
    }
}

// -----------------------
// Enum Bit-flags Operator
// -----------------------

GPUBufferMapping operator|(GPUBufferMapping a, GPUBufferMapping b) {
    return (GPUBufferMapping) ((int) a | (int) b);
}

GPUBufferMapping operator&(GPUBufferMapping a, GPUBufferMapping b) {
    return (GPUBufferMapping) ((int) a & (int) b);
}

GPUMemoryBarrier operator|(GPUMemoryBarrier a, GPUMemoryBarrier b) {
    return (GPUMemoryBarrier) ((int) a | (int) b);
}

GPUMemoryBarrier operator&(GPUMemoryBarrier a, GPUMemoryBarrier b) {
    return (GPUMemoryBarrier) ((int) a & (int) b);
}

// ------------------
// Dummy Initializers
// ------------------

// -- GPU Buffer --
GPUBuffer::GPUBuffer() = default;
GPUBuffer::~GPUBuffer() = default;
// -- GPU Vertex Array --
GPUVertexArray::GPUVertexArray() = default;
GPUVertexArray::~GPUVertexArray() = default;

// -- GPU Texture --
GPUTexture::GPUTexture() = default;
GPUTexture::~GPUTexture() = default;
