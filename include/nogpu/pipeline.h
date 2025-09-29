// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_PIPELINE_H
#define NOGPU_PIPELINE_H
#include "shader.h"

typedef struct {
    bool x, y, w, h;
} GPURectangle;

typedef struct {
    float r, g, b, a;
} GPUColor;

// ----------------------------
// GPU Pipeline: Blending State
// ----------------------------

enum class GPUBlendEquation : int {
    BLEND_FUNC_ADD,
    BLEND_FUNC_SUBSTRACT,
    BLEND_FUNC_REVERSE_SUBSTRACT,
    BLEND_FUNC_MIN,
    BLEND_FUNC_MAX
};

enum class GPUBlendFactor : int {
    BLEND_ZERO,
    BLEND_ONE,
    BLEND_SRC_COLOR,
    BLEND_ONE_MINUS_SRC_COLOR,
    BLEND_DST_COLOR,
    BLEND_ONE_MINUS_DST_COLOR,
    BLEND_SRC_ALPHA,
    BLEND_ONE_MINUS_SRC_ALPHA,
    BLEND_DST_ALPHA,
    BLEND_ONE_MINUS_DST_ALPHA,
    BLEND_CONSTANT_COLOR,
    BLEND_ONE_MINUS_CONSTANT_COLOR,
    BLEND_CONSTANT_ALPHA,
    BLEND_ONE_MINUS_CONSTANT_ALPHA,
    BLEND_SRC_ALPHA_SATURATE,
};

typedef struct GPUPipelineBlending {
    struct GPUPipelineBlendingEquation {
        GPUBlendEquation rgb;
        GPUBlendEquation alpha;
    } equation;

    struct GPUPipelineBlendingFactor {
        GPUBlendFactor srcRGB, srcAlpha;
        GPUBlendFactor dstRGB, dstAlpha;
    } factor;

    GPUColor color;
} GPUPipelineBlending;

// --------------------------
// GPU Pipeline: Face Winding
// --------------------------

enum class GPUFaceMode : int {
    FACE_BACK,
    FACE_FRONT,
    FACE_FRONT_AND_BACK
};

enum class GPUFaceWinding : int {
    FACE_WINDING_CW,
    FACE_WINDING_CCW,
};

typedef struct GPUPipelineFace {
    GPUFaceMode cull;
    GPUFaceWinding winding;
} GPUPipelineFace;

// ------------------------
// GPU Pipeline: Depth Mode
// ------------------------

enum class GPUConditionMode : int {
    CONDITION_NEVER,
    CONDITION_EQUAL,
    CONDITION_NOTEQUAL,
    CONDITION_LESS,
    CONDITION_LEQUAL,
    CONDITION_GREATER,
    CONDITION_GEQUAL,
    CONDITION_ALWAYS,
};

typedef struct GPUPipelineDepth {
    GPUConditionMode condition;
    unsigned int mask;

    struct GPUPipelineDepthBias {
        float constantFactor;
        float slopeFactor;
        float clamp;
    } bias;
} GPUPipelineDepth;

// -------------------------
// GPU Context: Stencil Mode
// -------------------------

enum class GPUStencilMode : int {
    STENCIL_KEEP,
    STENCIL_ZERO,
    STENCIL_REPLACE,
    STENCIL_INCR,
    STENCIL_INCR_WRAP,
    STENCIL_DECR,
    STENCIL_DECR_WRAP,
    STENCIL_INVERT
};

typedef struct GPUPipelineStencil {
    struct GPUPipelineStencilFunction {
        GPUFaceMode face;
        GPUConditionMode condition;
        unsigned int mask;
        int test;
    } function;

    struct GPUPipelineStencilMask {
        GPUFaceMode face;
        unsigned int mask;
    } mask;

    struct GPUPipelineStencilMode {
        GPUFaceMode face;
        GPUStencilMode fail;
        GPUStencilMode pass;
        GPUStencilMode depth_pass;
    } mode;
} GPUPipelineStencil;

// ---------------------
// GPU Context: Pipeline
// ---------------------

enum class GPUPipelineCapability : int {
    CAPABILITY_BLENDING,
    CAPABILITY_FACE_CULL,
    CAPABILITY_DEPTH,
    CAPABILITY_DEPTH_BIAS,
    CAPABILITY_SCISSOR,
    CAPABILITY_STENCIL,
    CAPABILITY_PRIMITIVE_RESTART,
    CAPABILITY_RASTERIZE_DISCARD,
    CAPABILITY_MULTISAMPLE,
};

class GPUPipeline {
    public: // Pipeline Capabilites
        virtual void destroy() = 0;
        virtual bool checkCapability(GPUPipelineCapability cap) = 0;
        virtual void enableCapability(GPUPipelineCapability cap) = 0;
        virtual void disableCapability(GPUPipelineCapability cap) = 0;

    public: // Pipeline Attributes: Setters
        virtual void setProgram(GPUProgram *program) = 0;
        virtual void setBlending(GPUPipelineBlending blending) = 0;
        virtual void setFace(GPUPipelineFace face) = 0;
        virtual void setDepth(GPUPipelineDepth depth) = 0;
        virtual void setStencil(GPUPipelineStencil stencil) = 0;
        // GPU Context State: Viewport
        virtual void setLineWidth(float width) = 0;
        virtual void setClearColor(GPUColor color) = 0;
        virtual void setMaskColor(GPUColor mask) = 0;
        virtual void setViewport(GPURectangle rect) = 0;
        virtual void setScissor(GPURectangle rect) = 0;

    public: // Pipeline Attributes: Getters
        virtual GPUProgram* getProgram() = 0;
        virtual GPUPipelineBlending getBlending() = 0;
        virtual GPUPipelineFace getFace() = 0;
        virtual GPUPipelineDepth getDepth() = 0;
        virtual GPUPipelineStencil getStencil() = 0;
        // GPU Context State: Viewport
        virtual float getLineWidth() = 0;
        virtual GPUColor getClearColor() = 0;
        virtual GPUColor getMaskColor() = 0;
        virtual GPURectangle getViewport() = 0;
        virtual GPURectangle getScissor() = 0;
};

#endif // NOGPU_PIPELINE_H
