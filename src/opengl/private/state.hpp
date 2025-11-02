// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_STATE_H
#define OPENGL_STATE_H
#include <nogpu/pipeline.h>
#include <nogpu_map.h>

// ---------------------
// OpenGL Pipeline State
// ---------------------

enum class GLPipelineEffect : unsigned int {
    PIPELINE_EFFECT_CAPABILITIES,
    PIPELINE_EFFECT_PROGRAM,
    PIPELINE_EFFECT_BLENDING,
    PIPELINE_EFFECT_CULLING,
    PIPELINE_EFFECT_DEPTH,
    PIPELINE_EFFECT_STENCIL,
    // OpenGL Viewport State
    PIPELINE_EFFECT_CLEAR_DEPTH,
    PIPELINE_EFFECT_CLEAR_STENCIL,
    PIPELINE_EFFECT_CLEAR_COLOR,
    PIPELINE_EFFECT_COLOR_MASK,
    PIPELINE_EFFECT_VIEWPORT,
    PIPELINE_EFFECT_SCISSOR,
    PIPELINE_EFFECT_LINE_WIDTH
};

class GLProgram;
typedef struct GLPipelineState {
    GLProgram* program;
    unsigned int capabilities;
    GPUPipelineBlending blending;
    GPUPipelineFace culling;
    GPUPipelineDepth depth;
    GPUPipelineStencil stencil;

    // OpenGL Viewport State
    float clear_depth;
    int clear_stencil;
    GPUColor clear_color;
    GPUColorMask color_mask;
    GPURectangle viewport;
    GPURectangle scissor;
    float line_width;
} GLPipelineState;

// --------------------
// OpenGL Command State
// --------------------

enum class GLCommandsEffect : unsigned int {
    CMD_EFFECT_PIPELINE,
    CMD_EFFECT_VERTEX_ARRAY,
    CMD_EFFECT_BLOCK_BINDING,
    CMD_EFFECT_TEXTURE_ACTIVE,
    CMD_EFFECT_FRAMEBUFFER_DRAW,
    CMD_EFFECT_FRAMEBUFFER_READ,
};

class GLPipeline;
class GLCommands;
class GLVertexArray;
class GLBuffer;
class GLTexture;
class GLFrameBuffer;
typedef struct GLCommandsState {
    GLPipeline *pipeline;
    GLVertexArray *vertex_array;
    GPUHashmap<GLBuffer*> block_bindings;
    GPUHashmap<GLTexture*> texture_actives;
    GLFrameBuffer *framebuffer_draw;
    GLFrameBuffer *framebuffer_read;
} GLCommandsState;

// -------------------
// OpenGL Global State
// -------------------

class GLState {
    public: // OpenGL Capabilites
        bool checkCapability(GPUPipelineCapability cap);
        void enableCapability(GPUPipelineCapability cap);
        void disableCapability(GPUPipelineCapability cap);
        bool resolveEffects();

    public: // OpenGL State: Pipeline
        unsigned int m_pipeline_effects = 0;
        GLPipelineState m_pipeline_state {};
        GLPipeline* m_pipeline_current = nullptr;
        GLPipeline* m_pipeline_cached = nullptr;
        void markPipelineEffect(GLPipelineEffect effect);
        void unmarkPipelineEffect(GLPipelineEffect effect);
        void checkPipelineEffects(GLPipelineState &check);

    public: // OpenGL State: Commands
        unsigned int m_stole_texture = 0;
        unsigned int m_commands_effects = 0;
        GLCommandsState m_commands_state {};
        GLCommands* m_commands_current = nullptr;
        GLCommands* m_commands_cached = nullptr;
        void markCommandsEffect(GLCommandsEffect effect);
        void unmarkCommandsEffect(GLCommandsEffect effect);
        void checkCommandsEffects(GLCommandsState &check);

    private: // OpenGL State: Effects
        void resolvePipelineEffect(GLPipelineEffect effect);
        void resolvePipelineEffects();
};

#endif // OPENGL_STATE_H
