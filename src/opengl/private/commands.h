// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_COMMANDS_H
#define OPENGL_COMMANDS_H
#include <nogpu/commands.h>

class GLContext;
class GLFence : GPUFence {
    // Fence Signaling
    void syncCPU() override;
    void syncGPU() override;

    protected:
        GLFence(GLContext* ctx);
        void destroy() override;
};

// -------------------
// OpenGL GPU Commands
// -------------------

class GLContext;
class GLCommands : GPUCommands {
    // GPU Command Record
    void beginCommands() override;
    void endCommands() override;
    void syncFlush() override;
    void syncFinish() override;
    virtual GPUFence* syncFence() override;

    // GPU Command State
    void usePipeline(GPUPipeline *pipeline) override;
    void useVertexArray(GPUVertexArray *vertex_array) override;
    void useTexture(GPUTexture *texture, int index) override;
    void useBlockBinding(GPUBuffer *buffer, GPUBlockBinding bind, int index) override;
    void useBlockBindingRange(GPUBuffer *buffer, GPUBlockBinding bind, int index, int offset, int size) override;
    void useFrameBuffer(GPUFrameBuffer *framebuffer) override;
    void useFrameBufferDraw(GPUFrameBuffer *framebuffer) override;
    void useFrameBufferRead(GPUFrameBuffer *framebuffer) override;
    void useFrameBufferDefault() override;

    // GPU Command Rendering
    void drawClear(GPUDrawClear clear) override;
    void drawArrays(GPUDrawPrimitive type, int offset, int count) override;
    void drawElements(GPUDrawPrimitive type, int offset, int count, GPUDrawElementsType element) override;
    void drawElementsBaseVertex(GPUDrawPrimitive type, int offset, int count, int base, GPUDrawElementsType element) override;
    void drawArraysInstanced(GPUDrawPrimitive type, int offset, int count, int instance_count) override;
    void drawElementsInstanced(GPUDrawPrimitive type, int offset, int count, GPUDrawElementsType element, int instance_count) override;
    void drawElementsBaseVertexInstanced(GPUDrawPrimitive type, int offset, int count, int base, GPUDrawElementsType element, int instance_count) override;
    void executeComputeSync(unsigned int num_groups_x, unsigned int num_groups_y, unsigned int num_groups_z) override;
    void executeCompute(unsigned int num_groups_x, unsigned int num_groups_y, unsigned int num_groups_z) override;
    void memoryBarrier(GPUMemoryBarrier from, GPUMemoryBarrier to) override;

    protected: // Commands Constructor
        GLCommands(GLContext ctx);
        void destroy() override;
        friend GLContext;
};

#endif // OPENGL_COMMANDS_H
