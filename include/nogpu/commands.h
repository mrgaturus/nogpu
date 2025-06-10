// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_COMMANDS_H
#define NOGPU_COMMANDS_H
#include "framebuffer.h"
#include "pipeline.h"

// ---------------------
// GPU Context: Commands
// ---------------------

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

enum class GPUMemoryBarrier : int {
    BARRIER_VEXTEX_ATTRIB_ARRAY = 1 << 0,
    BARRIER_ELEMENT_ARRAY = 1 << 1,
    BARRIER_UNIFORM = 1 << 2,
    BARRIER_TEXTURE_FETCH = 1 << 3,
    BARRIER_SHADER_IMAGE_ACCESS = 1 << 4,
    BARRIER_COMMAND = 1 << 5,
    BARRIER_PIXEL_BUFFER = 1 << 6,
    BARRIER_TEXTURE_UPDATE = 1 << 7,
    BARRIER_BUFFER_UPDATE = 1 << 8,
    BARRIER_FRAMEBUFFER = 1 << 9,
    BARRIER_ATOMIC_COUNTER = 1 << 10,
    BARRIER_SHADER_STORAGE = 1 << 11,
    BARRIER_ALL = 0x7FFFFFFF,
};

typedef struct {
    GPUPipeline *pipeline;
    GPUPipeline pipelineCache;
    GPUBuffer *buffers[16];
    GPUTexture *textures[64];
    GPUVertexArray *vertexArray;
    GPUFrameBuffer *framebufferRead;
    GPUFrameBuffer *framebufferDraw;
} GPUCommandState;

class GPUCommands {
    protected:
        GPUCommands();
        ~GPUCommands();
        GPUCommandState m_state;
        bool m_recorded;
    public: virtual void destroy() = 0;

    public: // GPU Command Record 
        bool checkRecord() { return m_recorded; }
        virtual void beginRecord() = 0;
        virtual void endRecord() = 0;
        virtual void wait() = 0;

    public: // GPU Command State
        virtual void usePipeline(GPUPipeline *pipeline) = 0;
        virtual void useVertexArray(GPUVertexArray *vertex_array) = 0;
        virtual void useTexture(GPUTexture *texture, int index) = 0;
        virtual void useFrameBuffer(GPUFrameBuffer* draw, GPUFrameBuffer* read) = 0;
        virtual void useFrameBuffer(GPUFrameBuffer* draw) = 0;
        virtual void useFrameBufferDefault() = 0;

    public: // GPU Command Rendering
        virtual void drawClear() = 0;
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
#endif // NOGPU_COMMANDS_H
