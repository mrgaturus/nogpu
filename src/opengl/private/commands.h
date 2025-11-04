// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_COMMANDS_H
#define OPENGL_COMMANDS_H
#include <nogpu/commands.h>
#include "state.hpp"

typedef int GLint;
typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef unsigned int GLbitfield;
typedef struct __GLsync *GLsync;

GLenum toValue(GPUBlockBinding type);
GLbitfield toValue(GPUDrawClear flags);
GLenum toValue(GPUDrawPrimitive primitive);
GLenum toValue(GPUDrawElements mode);
GLbitfield toValue(GPUMemoryBarrier flags);

class GLContext;
class GLCommands;
class GLFence : GPUFence {
    GLContext* m_ctx;
    GLsync m_sync;
    // Fence Syncronization
    void waitCPU() override;
    void waitGPU() override;
    bool completed() override;

    protected:
        friend GLContext;
        friend GLCommands;
        GLFence(GLContext* ctx);
        void destroy() override;
};

// -------------------
// OpenGL GPU Commands
// -------------------

class GLCommands : GPUCommands {
    GLCommandsState m_state;
    GLState *m_ctx_state;
    GLContext* m_ctx;

    // GPU Command Record
    void beginCommands() override;
    void endCommands() override;
    // GPU Command Fence
    void syncFlush() override;
    void syncFinish() override;
    GPUFence* syncFence() override;

    // GPU Command State
    void usePipeline(GPUPipeline *pipeline) override;
    void useVertexArray(GPUVertexArray *vertex_array) override;
    void useBlockBinding(GPUBuffer *buffer, GPUBlockBinding bind, int index) override;
    void useBlockBindingRange(GPUBuffer *buffer, GPUBlockBinding bind, int index, int offset, int size) override;
    void useTexture(GPUTexture *texture, int index) override;
    void useFrameBuffer(GPUFrameBuffer *framebuffer) override;
    void useFrameBufferDraw(GPUFrameBuffer *framebuffer) override;
    void useFrameBufferRead(GPUFrameBuffer *framebuffer) override;
    void useFrameBufferDefault() override;

    // GPU Command Rendering
    void drawClear(GPUDrawClear clear) override;
    void drawArrays(GPUDrawPrimitive type, int offset, int count) override;
    void drawElements(GPUDrawPrimitive type, int offset, int count, GPUDrawElements element) override;
    void drawElementsBaseVertex(GPUDrawPrimitive type, int offset, int count, int base, GPUDrawElements element) override;
    void drawArraysInstanced(GPUDrawPrimitive type, int offset, int count, int instance_count) override;
    void drawElementsInstanced(GPUDrawPrimitive type, int offset, int count, GPUDrawElements element, int instance_count) override;
    void drawElementsBaseVertexInstanced(GPUDrawPrimitive type, int offset, int count, int base, GPUDrawElements element, int instance_count) override;
    void executeComputeSync(int x, int y, int z) override;
    void executeCompute(int x, int y, int z) override;
    void memoryBarrier(GPUMemoryBarrier from, GPUMemoryBarrier to) override;

    protected: // Commands Constructor
        GLCommands(GLContext ctx);
        void destroy() override;
        friend GLContext;
};

#endif // OPENGL_COMMANDS_H
