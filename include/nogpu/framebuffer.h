// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_FRAMEBUFFER_H
#define NOGPU_FRAMEBUFFER_H
#include "texture.h"

// ------------------------
// GPU Objects: Framebuffer
// ------------------------

enum class GPURenderBufferMode : int {
    RENDERBUFFER_OFFSCREEN,
    RENDERBUFFER_TEXTURE,
    RENDERBUFFER_TEXTURE_ARRAY,
    RENDERBUFFER_TARGET,
    RENDERBUFFER_TARGET_ARRAY,
    RENDERBUFFER_TARGET_CUBEMAP,
    RENDERBUFFER_TARGET_CUBEMAP_ARRAY
};

class GPURenderBuffer {
    protected:
        GPUTexturePixelType m_pixel_type;
        GPURenderBufferMode m_mode;
        GPUTexture* m_texture;

    public: // Renderbuffer Manipulation
        virtual void destroy() = 0;
        virtual void useTexture(GPUTexture* texture) = 0;
        virtual void createTexture(int w, int h, int samples) = 0;
        virtual void createTextureArray(int w, int h, int samples, int layers) = 0;
        virtual void createOffscreen(int w, int h, int samples) = 0;

    public: // Renderbuffer Attributes
        virtual int getSamples() = 0;
        virtual void getWidth() = 0;
        virtual void getHeight() = 0;
        virtual int getLayers() = 0;
        virtual GPUTextureSize getSize() = 0;
        GPUTexturePixelType getPixelType() { return m_pixel_type; }
        GPURenderBufferMode getMode() { return m_mode; }
        GPUTexture* getTexture() { return m_texture; }
};

enum class GPUFrameBufferStatus : int {
    FRAMEBUFFER_UNDEFINED,
    FRAMEBUFFER_UNSUPPORTED,
    FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
    FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
    FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
};

class GPUFrameBuffer {
    public: // Framebuffer Manipulation
        virtual void destroy() = 0;
        virtual void attachColor(GPURenderBuffer *target, int index) = 0;
        virtual void attachStencil(GPURenderBuffer *target) = 0;
        virtual void attachDepth(GPURenderBuffer *target) = 0;
    public: // Framebuffer Attributes
        virtual GPUFrameBufferStatus checkStatus() = 0;
        virtual GPURenderBuffer* getColor(int index) = 0;
        virtual GPURenderBuffer* getStencil() = 0;
        virtual GPURenderBuffer* getDepth() = 0;
};

#endif // NOGPU_FRAMEBUFFER_H
