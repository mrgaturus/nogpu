// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_FRAMEBUFFER_H
#define NOGPU_FRAMEBUFFER_H
#include "texture.h"

// ------------------------
// GPU Objects: FrameBuffer
// ------------------------

enum class GPURenderBufferMode : int {
    RENDERBUFFER_TEXTURE,
    RENDERBUFFER_TEXTURE_ARRAY,
    RENDERBUFFER_OFFSCREEN
};

class GPURenderBuffer {
    protected:
        GPURenderBufferMode m_mode;
        GPUTexturePixelFormat m_format;
        int m_msaa_samples;
        int m_w, m_h;
    // GPU Renderbuffer Constructor
    protected: GPURenderBuffer();
    protected: ~GPURenderBuffer();
    public: virtual void destroy() = 0;
    public: virtual void allocate(int w, int h, int msaa_samples) = 0;
    public: virtual void allocate(int w, int h, int layers, int msaa_samples) = 0;

    public: // Renderbuffer Attributes
        int getW() { return m_w; }
        int getH() { return m_h; }
        int getMultisamples() { return m_msaa_samples; }
        GPUTextureSize getSize() { return (GPUTextureSize) { m_w, m_h }; }
        GPURenderBufferMode getMode() { return m_mode; }
        GPUTexturePixelFormat getFormat() { return m_format; }
        virtual GPUTexture* getTexture();
};

enum class GPUFrameBufferStatus : int {
    FRAMEBUFFER_UNDEFINED,
    FRAMEBUFFER_UNSUPPORTED,
    FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
    FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
    FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
};

class GPUFrameBuffer {
    protected:
        GPUTexture *m_colors[64];
        GPUTexture *m_stencil;
        GPUTexture *m_depth;
    // GPU Framebuffer Constructor
    protected: GPUFrameBuffer();
    protected: ~GPUFrameBuffer();
    public: virtual void destroy() = 0;

    public: // GPU Texture Attach
        virtual GPUFrameBufferStatus checkStatus() = 0;
        virtual void attachColor(GPUTexture *color, int index) = 0;
        virtual void attachDepthStencil(GPUTexture *depth_stencil) = 0;
        virtual void attachStencil(GPUTexture *stencil) = 0;
        virtual void attachDepth(GPUTexture *depth) = 0;
        // GPU Renderbuffer Attachment
        virtual void attachColor(GPURenderBuffer *color, int index) = 0;
        virtual void attachDepthStencil(GPURenderBuffer *depth_stencil) = 0;
        virtual void attachStencil(GPURenderBuffer *stencil) = 0;
        virtual void attachDepth(GPURenderBuffer *depth) = 0;
    public: // GPU Framebuffer Attributes
        GPUTexture *getColor(int index) { return m_colors[index]; }
        GPUTexture *getStencil() { return m_stencil; }
        GPUTexture *getDepth() { return m_depth; }
};

#endif // NOGPU_FRAMEBUFFER_H
