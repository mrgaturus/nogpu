// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_COMMANDS_H
#define OPENGL_COMMANDS_H
#include <nogpu/commands.h>

// -------------------
// OpenGL GPU Commands
// -------------------

class GLContext;
class GLCommands : GPUCommands {
    // GPU Command Record
    void beginRecord() override;
    void endRecord() override;
    void wait() override;

    // GPU Command State
    void usePipeline(GPUPipeline *pipeline) override;
    void useVertexArray(GPUVertexArray *vertex_array) override;
    void useTexture(GPUTexture *texture, int index) override;
    void useFrameBuffer(GPUFrameBuffer* draw) override;
    void useFrameBuffer(GPUFrameBuffer* draw, GPUFrameBuffer* read) override;
    void useFrameBufferDefault() override;

    // GPU Command Rendering
    void drawClear() override;
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
        GLCommands();
        void destroy() override;
        friend GLContext;
};

#endif // OPENGL_COMMANDS_H
