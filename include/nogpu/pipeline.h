// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_PIPELINE_H
#define NOGPU_PIPELINE_H
#include "shader.h"

// -----------------------------
// GPU Context: State Primitives
// -----------------------------

typedef struct {
    bool x, y, w, h;
} GPURectangle;

typedef struct {
    float r, g, b, a;
} GPUColor;

typedef struct {
    bool r, g, b, a;
} GPUColorMask;

// ---------------------------
// GPU Context: Pipeline State
// ---------------------------

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

enum class GPUConditionFunc : int {
    CONDITION_NEVER,
    CONDITION_EQUAL,
    CONDITION_NOTEQUAL,
    CONDITION_LESS,
    CONDITION_LEQUAL,
    CONDITION_GREATER,
    CONDITION_GEQUAL,
    CONDITION_ALWAYS,
};

enum class GPUStencilFunc : int {
    STENCIL_KEEP,
    STENCIL_ZERO,
    STENCIL_REPLACE,
    STENCIL_INCR,
    STENCIL_INCR_WRAP,
    STENCIL_DECR,
    STENCIL_DECR_WRAP,
    STENCIL_INVERT
};

enum class GPUFaceMode : int {
    FACE_BACK,
    FACE_FRONT,
    FACE_FRONT_AND_BACK
};

enum class GPUWindingMode : int {
    WINDING_CW,
    WINDING_CCW,
};

// ---------------------
// GPU Context: Pipeline
// ---------------------

enum class GPUPipelineCapability : int {
    CAPABILITY_BLENDING,
    CAPABILITY_FACE_CULL,
    CAPABILITY_DEPTH_TEST,
    CAPABILITY_DEPTH_BIAS,
    CAPABILITY_PRIMITIVE_RESTART,
    CAPABILITY_RASTERIZER_DISCARD,
    CAPABILITY_SCISSOR_TEST,
    CAPABILITY_STENCIL_TEST,
    CAPABILITY_MULTISAMPLE,
};

typedef struct {
    struct GPUPipelineBlendingEquation {
        GPUBlendEquation rgb;
        GPUBlendEquation alpha;
    } equation;

    struct GPUPipelineBlendingFactor {
        GPUBlendFactor srcRGB, srcAlpha;
        GPUBlendFactor dstRGB, dstAlpha;
    } factor;

    GPUColor constantColor;
} GPUPipelineBlending;

typedef struct {
    GPUFaceMode cull;
    GPUWindingMode front;
} GPUPipelineFace;

typedef struct {
    GPUConditionFunc func;
    bool mask;

    struct GPUPipelineDepthBias {
        float constantFactor;
        float slopeFactor;
        float clamp;
    } bias;
} GPUPipelineDepth;

typedef struct {
    struct GPUPipelineStencilFunc {
        GPUFaceMode face;
        GPUConditionFunc func;
        int test; unsigned int mask;
    } func;

    struct GPUPipelineStencilMask {
        GPUFaceMode face;
        unsigned int mask;
    } mask;

    struct GPUPipelineStencilOp {
        GPUFaceMode face;
        GPUStencilFunc fail;
        GPUStencilFunc pass;
        GPUStencilFunc depth_pass;
    } op;
} GPUPipelineStencil;

class GPUPipeline {
    GPUProgram* m_program;

    unsigned int m_capabilities;
    GPUPipelineBlending m_blending;
    GPUPipelineFace m_face;
    GPUPipelineDepth m_depth;
    GPUPipelineStencil m_stencil;

    float m_lineWidth;
    GPUColor m_clearColor;
    GPUColorMask m_maskColor;
    GPURectangle m_scissor;
    GPURectangle m_viewport;

    // Pipeline Constructor
    protected:
        GPUPipeline(GPUProgram* program);
        ~GPUPipeline();
    public: virtual void destroy() { delete this; };

    public: // Pipeline Capabilites
        bool check(GPUPipelineCapability cap) { return (m_capabilities & (1 << (unsigned int) cap)) != 0; }
        virtual void enable(GPUPipelineCapability cap) { m_capabilities |= (1 << (unsigned int) cap); }
        virtual void disable(GPUPipelineCapability cap) { m_capabilities &= ~(1 << (unsigned int) cap); }

    public: // Pipeline Attributes: Setters
        virtual void setProgram(GPUProgram* program) { m_program = program; };
        virtual void setBlending(GPUPipelineBlending blending) { m_blending = blending; }
        virtual void setFace(GPUPipelineFace face) { m_face = face; }
        virtual void setDepth(GPUPipelineDepth depth) { m_depth = depth; }
        virtual void setStencil(GPUPipelineStencil stencil) { m_stencil = stencil; }
        // GPU Context State: Viewport
        virtual void setLineWidth(float width) { m_lineWidth = width; }
        virtual void setClearColor(GPUColor color) { m_clearColor = color; }
        virtual void setMaskColor(GPUColorMask mask) { m_maskColor = mask; }
        virtual void setViewport(GPURectangle rect) { m_viewport = rect; }
        virtual void setScissor(GPURectangle rect) { m_scissor = rect; }

    public: // Pipeline Attributes: Getters
        GPUPipelineBlending getBlending() { return m_blending; }
        GPUPipelineFace getFace() { return m_face; }
        GPUPipelineDepth getDepth() { return m_depth; }
        GPUPipelineStencil getStencil() { return m_stencil; }
        // GPU Context State: Viewport
        float getLineWidth() { return m_lineWidth; }
        GPUColor getClearColor() { return m_clearColor; }
        GPUColorMask getMaskColor() { return m_maskColor; }
        GPURectangle getViewport() { return m_viewport; }
        GPURectangle getScissor() { return m_scissor; }
};

#endif // NOGPU_PIPELINE_H
