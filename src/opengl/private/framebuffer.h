// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_FRAMEBUFFER_H
#define OPENGL_FRAMEBUFFER_H
#include <nogpu/framebuffer.h>

// ----------------------
// OpenGL GPU FrameBuffer
// ----------------------

class GLContext;
class GLRenderBuffer : GPURenderBuffer {
    protected: // RenderBuffer Constructor
        GLRenderBuffer(GPURenderBufferMode mode, GPUTextureTransferFormat format);
        void allocate(int w, int h, int msaa_samples) override;
        void allocate(int w, int h, int layers, int msaa_samples) override;

        GPUTexture* getTexture() override;
        void destroy() override;
        friend GLContext;
};

class GLFrameBuffer : GPUFrameBuffer {
    GPUFrameBufferStatus checkStatus() override;
    void attachColor(GPUTexture *color, int index) override;
    void attachDepthStencil(GPUTexture *depth_stencil) override;
    void attachStencil(GPUTexture *stencil) override;
    void attachDepth(GPUTexture *depth) override;
    // GPU RenderBuffer Attachment
    void attachColor(GPURenderBuffer *color, int index) override;
    void attachDepthStencil(GPURenderBuffer *depth_stencil) override;
    void attachStencil(GPURenderBuffer *stencil) override;
    void attachDepth(GPURenderBuffer *depth) override;

    protected: // FrameBuffer Constructor
        GLFrameBuffer();
        void destroy() override;
        friend GLContext;
};

#endif // OPENGL_FRAMEBUFFER_H
