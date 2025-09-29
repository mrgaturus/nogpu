// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_COMMANDS_H
#define NOGPU_COMMANDS_H
#include "framebuffer.h"
#include "pipeline.h"

// --------------------------
// GPU Context: Command Enums
// --------------------------

enum class GPUBlockBinding : int {
    BLOCK_BUFFER_OBJECT,
    BLOCK_SHADER_STORAGE,
    BLOCK_ATOMIC_COUNTER,
};

enum class GPUDrawClear : unsigned int {
    CLEAR_COLOR = 1 << 0,
    CLEAR_DEPTH = 1 << 1,
    CLEAR_STENCIL = 1 << 2
};

enum class GPUDrawPrimitive : int {
    PRIMITIVE_POINTS,
    PRIMITIVE_LINES,
    PRIMITIVE_LINE_STRIP,
    PRIMITIVE_LINE_LOOP,
    PRIMITIVE_TRIANGLES,
    PRIMITIVE_TRIANGLE_STRIP,
    PRIMITIVE_TRIANGLE_FAN,
};

enum class GPUDrawElementsType : int {
    ELEMENTS_UNSIGNED_BYTE,
    ELEMENTS_UNSIGNED_SHORT,
    ELEMENTS_UNSIGNED_INT
};

// ---------------------------
// GPU Context: Syncronization
// ---------------------------

enum class GPUMemoryBarrier : unsigned int {
    BARRIER_BUFFER = 1 << 0,
    BARRIER_TEXTURE = 1 << 1,
    BARRIER_TRANSFER = 1 << 2,
    BARRIER_RENDER = 1 << 3,
    BARRIER_COMPUTE = 1 << 4,
    BARRIER_ALL = 0xFFFFFFFF
};

class GPUFence {
    public: // Fence Signaling
        virtual void destroy() = 0;
        virtual void syncCPU() = 0;
        virtual void syncGPU() = 0;
};

// ---------------------
// GPU Context: Commands
// ---------------------

class GPUCommands {
    public: // GPU Command Issue
        virtual void destroy() = 0;
        virtual void beginCommands() = 0;
        virtual void endCommands() = 0;
        virtual void syncFlush() = 0;
        virtual void syncFinish() = 0;
        virtual GPUFence* syncFence() = 0;

    public: // GPU Command State
        virtual void usePipeline(GPUPipeline *pipeline) = 0;
        virtual void useVertexArray(GPUVertexArray *vertex) = 0;
        virtual void useTexture(GPUTexture *texture, int index) = 0;
        virtual void useBlockBinding(GPUBuffer *buffer, GPUBlockBinding bind, int index) = 0;
        virtual void useBlockBindingRange(GPUBuffer *buffer, GPUBlockBinding bind, int index, int offset, int size) = 0;
        virtual void useFrameBuffer(GPUFrameBuffer *framebuffer) = 0;
        virtual void useFrameBufferDraw(GPUFrameBuffer *framebuffer) = 0;
        virtual void useFrameBufferRead(GPUFrameBuffer *framebuffer) = 0;
        virtual void useFrameBufferDefault() = 0;

    public: // GPU Command Rendering
        virtual void drawClear(GPUDrawClear clear) = 0;
        virtual void drawArrays(GPUDrawPrimitive type, int offset, int count) = 0;
        virtual void drawElements(GPUDrawPrimitive type, int offset, int count, GPUDrawElementsType element) = 0;
        virtual void drawElementsBaseVertex(GPUDrawPrimitive type, int offset, int count, int base, GPUDrawElementsType element) = 0;
        virtual void drawArraysInstanced(GPUDrawPrimitive type, int offset, int count, int instance_count) = 0;
        virtual void drawElementsInstanced(GPUDrawPrimitive type, int offset, int count, GPUDrawElementsType element, int instance_count) = 0;
        virtual void drawElementsBaseVertexInstanced(GPUDrawPrimitive type, int offset, int count, int base, GPUDrawElementsType element, int instance_count) = 0;
        virtual void executeComputeSync(unsigned int num_groups_x, unsigned int num_groups_y, unsigned int num_groups_z) = 0;
        virtual void executeCompute(unsigned int num_groups_x, unsigned int num_groups_y, unsigned int num_groups_z) = 0;
        virtual void memoryBarrier(GPUMemoryBarrier from, GPUMemoryBarrier to) = 0;
};

GPUMemoryBarrier operator|(GPUMemoryBarrier a, GPUMemoryBarrier b);
GPUMemoryBarrier operator&(GPUMemoryBarrier a, GPUMemoryBarrier b);
GPUDrawClear operator|(GPUDrawClear a, GPUDrawClear b);
GPUDrawClear operator&(GPUDrawClear a, GPUDrawClear b);
#endif // NOGPU_COMMANDS_H
