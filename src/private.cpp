// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "nogpu_private.h"
#include <nogpu/buffer.h>
#include <nogpu/program.h>
#include <nogpu/commands.h>
#include <cmath>

// -----------------------
// Power of Two Operations
// -----------------------

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

unsigned int levels_power_of_two(int w, int h, int limit) {
    if (w <= 0 || h <= 0)
        return 0;

    // Calculate Mipmap Levels
    double levels0 = (double) ((w > h) ? w : h);
    levels0 = log2(levels0) + 1;
    // Clamp With Levels Limit
    int levels = (unsigned int) levels0;
    if (limit > 0 && limit < levels)
        levels = limit;

    // Return Calculated Levels
    return levels;
}

// --------------------
// Device Feature Flags
// --------------------

unsigned int driver_option_flag(GPUDriverOption driver) {
    return 1 << static_cast<unsigned int>(driver);
}

unsigned int driver_feature_flag(GPUDriverFeature feature) {
    return 1 << static_cast<unsigned int>(feature);
}

// -----------------------
// Enum Bit-flags Operator
// -----------------------

GPUBufferMapping operator|(GPUBufferMapping a, GPUBufferMapping b) {
    return (GPUBufferMapping) (static_cast<int>(a) | static_cast<int>(b));
}

GPUBufferMapping operator&(GPUBufferMapping a, GPUBufferMapping b) {
    return (GPUBufferMapping) (static_cast<int>(a) & static_cast<int>(b));
}

GPUMemoryBarrier operator|(GPUMemoryBarrier a, GPUMemoryBarrier b) {
    return (GPUMemoryBarrier) (static_cast<int>(a) | static_cast<int>(b));
}

GPUMemoryBarrier operator&(GPUMemoryBarrier a, GPUMemoryBarrier b) {
    return (GPUMemoryBarrier) (static_cast<int>(a) & static_cast<int>(b));
}

// ---------------------
// Uniform Size in Bytes
// ---------------------

int computeUniformBytes(GPUUniformType type) {
    switch (type) {
        case GPUUniformType::UNIFORM_TEXTURE_SAMPLER:
        case GPUUniformType::UNIFORM_BLOCK_BUFFER_OBJECT:
        case GPUUniformType::UNIFORM_BLOCK_SHADER_STORAGE:
        case GPUUniformType::UNIFORM_BLOCK_ATOMIC_COUNTER:
            return sizeof(unsigned int) * 1;

        // Uniform Primitives Sizes
        case GPUUniformType::UNIFORM_BOOL:
        case GPUUniformType::UNIFORM_INT:
        case GPUUniformType::UNIFORM_UNSIGNED_INT:
        case GPUUniformType::UNIFORM_FLOAT:
            return sizeof(unsigned int) * 1;
        case GPUUniformType::UNIFORM_BOOL_x2:
        case GPUUniformType::UNIFORM_INT_x2:
        case GPUUniformType::UNIFORM_UNSIGNED_INT_x2:
        case GPUUniformType::UNIFORM_FLOAT_x2:
            return sizeof(unsigned int) * 2;
        case GPUUniformType::UNIFORM_BOOL_x3:
        case GPUUniformType::UNIFORM_INT_x3:
        case GPUUniformType::UNIFORM_UNSIGNED_INT_x3:
        case GPUUniformType::UNIFORM_FLOAT_x3:
            return sizeof(unsigned int) * 3;
        case GPUUniformType::UNIFORM_BOOL_x4:
        case GPUUniformType::UNIFORM_INT_x4:
        case GPUUniformType::UNIFORM_UNSIGNED_INT_x4:
        case GPUUniformType::UNIFORM_FLOAT_x4:
            return sizeof(unsigned int) * 4;

        // Uniform Matrix Sizes: Square
        case GPUUniformType::UNIFORM_MATRIX_2x2:
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_2x2:
            return sizeof(unsigned int) * 2 * 2;
        case GPUUniformType::UNIFORM_MATRIX_3x3:
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_3x3:
            return sizeof(unsigned int) * 3 * 3;
        case GPUUniformType::UNIFORM_MATRIX_4x4:
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_4x4:
            return sizeof(unsigned int) * 4 * 4;
        
        // Uniform Matrix Sizes: Rectangles
        case GPUUniformType::UNIFORM_MATRIX_2x3:
        case GPUUniformType::UNIFORM_MATRIX_3x2:
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_2x3:
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_3x2:
            return sizeof(unsigned int) * 2 * 3;
        case GPUUniformType::UNIFORM_MATRIX_2x4:
        case GPUUniformType::UNIFORM_MATRIX_4x2:
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_2x4:
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_4x2:
            return sizeof(unsigned int) * 2 * 4;
        case GPUUniformType::UNIFORM_MATRIX_3x4:
        case GPUUniformType::UNIFORM_MATRIX_4x3:
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_3x4:
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_4x3:
            return sizeof(unsigned int) * 3 * 4;
    }

    // Invalid Bytes
    return 0;
}
