// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "private/pipeline.h"

// ----------------------------
// GPU Pipeline: Blending State
// ----------------------------

GLenum toValue(GPUBlendEquation equation) {
    switch (equation) {
        case GPUBlendEquation::BLEND_FUNC_ADD:
            return GL_FUNC_ADD;
        case GPUBlendEquation::BLEND_FUNC_SUBSTRACT:
            return GL_FUNC_SUBTRACT;
        case GPUBlendEquation::BLEND_FUNC_REVERSE_SUBSTRACT:
            return GL_FUNC_REVERSE_SUBTRACT;
        case GPUBlendEquation::BLEND_FUNC_MIN:
            return GL_MIN;
        case GPUBlendEquation::BLEND_FUNC_MAX:
            return GL_MAX;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

GLenum toValue(GPUBlendFactor factor) {
    switch (factor) {
        case GPUBlendFactor::BLEND_ZERO:
            return GL_ZERO;
        case GPUBlendFactor::BLEND_ONE:
            return GL_ONE;

        case GPUBlendFactor::BLEND_SRC_COLOR:
            return GL_SRC_COLOR;
        case GPUBlendFactor::BLEND_ONE_MINUS_SRC_COLOR:
            return GL_ONE_MINUS_SRC_COLOR;
        case GPUBlendFactor::BLEND_DST_COLOR:
            return GL_DST_COLOR;
        case GPUBlendFactor::BLEND_ONE_MINUS_DST_COLOR:
            return GL_ONE_MINUS_DST_COLOR;
        case GPUBlendFactor::BLEND_SRC_ALPHA:
            return GL_SRC_ALPHA;
        case GPUBlendFactor::BLEND_ONE_MINUS_SRC_ALPHA:
            return GL_ONE_MINUS_SRC_ALPHA;
        case GPUBlendFactor::BLEND_DST_ALPHA:
            return GL_DST_ALPHA;
        case GPUBlendFactor::BLEND_ONE_MINUS_DST_ALPHA:
            return GL_ONE_MINUS_DST_ALPHA;

        case GPUBlendFactor::BLEND_CONSTANT_COLOR:
            return GL_CONSTANT_COLOR;
        case GPUBlendFactor::BLEND_ONE_MINUS_CONSTANT_COLOR:
            return GL_ONE_MINUS_CONSTANT_COLOR;
        case GPUBlendFactor::BLEND_CONSTANT_ALPHA:
            return GL_CONSTANT_ALPHA;
        case GPUBlendFactor::BLEND_ONE_MINUS_CONSTANT_ALPHA:
            return GL_ONE_MINUS_CONSTANT_ALPHA;

        case GPUBlendFactor::BLEND_SRC_ALPHA_SATURATE:
            return GL_SRC_ALPHA_SATURATE;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

// --------------------------
// GPU Pipeline: Face Winding
// --------------------------

GLenum toValue(GPUFaceMode mode) {
    switch (mode) {
        case GPUFaceMode::FACE_BACK:
            return GL_BACK;
        case GPUFaceMode::FACE_FRONT:
            return GL_FRONT;
        case GPUFaceMode::FACE_BOTH:
            return GL_FRONT_AND_BACK;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

GLenum toValue(GPUFaceWinding winding) {
    switch (winding) {
        case GPUFaceWinding::FACE_WINDING_CCW:
            return GL_CCW;
        case GPUFaceWinding::FACE_WINDING_CW:
            return GL_CW;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

// ----------------------------------
// GPU Pipeline: Depth & Stencil Mode
// ----------------------------------

GLenum toValue(GPUConditionMode condition) {
    switch (condition) {
        case GPUConditionMode::CONDITION_NEVER:
            return GL_NEVER;

        case GPUConditionMode::CONDITION_EQUAL:
            return GL_EQUAL;
        case GPUConditionMode::CONDITION_NOTEQUAL:
            return GL_NOTEQUAL;
        case GPUConditionMode::CONDITION_LESS:
            return GL_LESS;
        case GPUConditionMode::CONDITION_LEQUAL:
            return GL_LEQUAL;
        case GPUConditionMode::CONDITION_GREATER:
            return GL_GREATER;
        case GPUConditionMode::CONDITION_GEQUAL:
            return GL_GEQUAL;

        case GPUConditionMode::CONDITION_ALWAYS:
            return GL_ALWAYS;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

GLenum toValue(GPUStencilMode mode) {
    switch (mode) {
        case GPUStencilMode::STENCIL_KEEP:
            return GL_KEEP;
        case GPUStencilMode::STENCIL_ZERO:
            return GL_ZERO;
        case GPUStencilMode::STENCIL_REPLACE:
            return GL_REPLACE;
        case GPUStencilMode::STENCIL_INCR:
            return GL_INCR;
        case GPUStencilMode::STENCIL_INCR_WRAP:
            return GL_INCR_WRAP;
        case GPUStencilMode::STENCIL_DECR:
            return GL_DECR;
        case GPUStencilMode::STENCIL_DECR_WRAP:
            return GL_DECR_WRAP;
        case GPUStencilMode::STENCIL_INVERT:
            return GL_INVERT;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}
