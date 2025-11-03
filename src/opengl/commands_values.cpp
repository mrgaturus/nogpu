// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "private/commands.h"

GLenum toValue(GPUBlockBinding type) {
    switch (type) {
        case GPUBlockBinding::BLOCK_BUFFER_OBJECT:
            return GL_UNIFORM_BUFFER;
        case GPUBlockBinding::BLOCK_SHADER_STORAGE:
            return GL_SHADER_STORAGE_BUFFER;
        case GPUBlockBinding::BLOCK_ATOMIC_COUNTER:
            return GL_ATOMIC_COUNTER_BUFFER;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

// -----------------------
// GPU Context: Draw Modes
// -----------------------

GLbitfield toValue(GPUDrawClear flags) {
    GLbitfield result = 0;
    GLbitfield mask = (unsigned int) (flags);
    if (mask & (unsigned int) GPUDrawClear::CLEAR_COLOR)
        result |= GL_COLOR_BUFFER_BIT;
    if (mask & (unsigned int) GPUDrawClear::CLEAR_DEPTH)
        result |= GL_DEPTH_BUFFER_BIT;
    if (mask & (unsigned int) GPUDrawClear::CLEAR_STENCIL)
        result |= GL_STENCIL_BUFFER_BIT;

    // Return Clear Flags
    return result;
}

GLenum toValue(GPUDrawPrimitive primitive) {
    switch (primitive) {
        case GPUDrawPrimitive::PRIMITIVE_POINTS:
            return GL_POINTS;
        case GPUDrawPrimitive::PRIMITIVE_LINES:
            return GL_LINES;
        case GPUDrawPrimitive::PRIMITIVE_LINE_STRIP:
            return GL_LINE_STRIP;
        case GPUDrawPrimitive::PRIMITIVE_LINE_LOOP:
            return GL_LINE_LOOP;
        case GPUDrawPrimitive::PRIMITIVE_TRIANGLES:
            return GL_TRIANGLES;
        case GPUDrawPrimitive::PRIMITIVE_TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;
        case GPUDrawPrimitive::PRIMITIVE_TRIANGLE_FAN:
            return GL_TRIANGLE_FAN;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

GLenum toValue(GPUDrawElements mode) {
    switch (mode) {
        case GPUDrawElements::ELEMENTS_UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        case GPUDrawElements::ELEMENTS_UNSIGNED_SHORT:
            return GL_UNSIGNED_SHORT;
        case GPUDrawElements::ELEMENTS_UNSIGNED_INT:
            return GL_UNSIGNED_INT;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

// ---------------------------
// GPU Context: Syncronization
// ---------------------------

GLbitfield toValue(GPUMemoryBarrier flags) {
    GLbitfield result = 0;
    GLbitfield mask = (unsigned int) (flags);
    if (mask & (unsigned int) GPUMemoryBarrier::BARRIER_BUFFER)
        result |=
            GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT |
            GL_ELEMENT_ARRAY_BARRIER_BIT |
            GL_UNIFORM_BARRIER_BIT |
            GL_BUFFER_UPDATE_BARRIER_BIT;
    if (mask & (unsigned int) GPUMemoryBarrier::BARRIER_TEXTURE)
        result |=
            GL_TEXTURE_FETCH_BARRIER_BIT |
            GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
            GL_PIXEL_BUFFER_BARRIER_BIT |
            GL_TEXTURE_UPDATE_BARRIER_BIT;
    if (mask & (unsigned int) GPUMemoryBarrier::BARRIER_TRANSFER)
        result |=
            GL_PIXEL_BUFFER_BARRIER_BIT |
            GL_TEXTURE_UPDATE_BARRIER_BIT |
            GL_BUFFER_UPDATE_BARRIER_BIT;
    if (mask & (unsigned int) GPUMemoryBarrier::BARRIER_RENDER)
        result |=
            GL_COMMAND_BARRIER_BIT |
            GL_FRAMEBUFFER_BARRIER_BIT;
    if (mask & (unsigned int) GPUMemoryBarrier::BARRIER_COMPUTE)
        result |=
            GL_COMMAND_BARRIER_BIT |
            GL_SHADER_IMAGE_ACCESS_BARRIER_BIT |
            GL_ATOMIC_COUNTER_BARRIER_BIT |
            GL_SHADER_STORAGE_BARRIER_BIT;

    // Return Barrier Flags
    return result;
}
