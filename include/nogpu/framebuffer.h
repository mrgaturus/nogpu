// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_FRAMEBUFFER_H
#define NOGPU_FRAMEBUFFER_H
#include "texture.h"

// -------------------------------
// GPU Objects: Framebuffer Target
// -------------------------------

enum class GPURenderBufferMode : int {
    RENDERBUFFER_UNDEFINED,
    RENDERBUFFER_OFFSCREEN,
    RENDERBUFFER_TEXTURE,
    RENDERBUFFER_TEXTURE_3D,
    RENDERBUFFER_TEXTURE_ARRAY,
    RENDERBUFFER_TEXTURE_MULTISAMPLE,
    RENDERBUFFER_TEXTURE_MULTISAMPLE_ARRAY,
    RENDERBUFFER_TARGET,
    RENDERBUFFER_TARGET_3D,
    RENDERBUFFER_TARGET_ARRAY,
    RENDERBUFFER_TARGET_CUBEMAP,
    RENDERBUFFER_TARGET_CUBEMAP_ARRAY
};

class GPURenderBuffer {
    protected:
        GPUTexturePixelType m_pixel_type;
        GPURenderBufferMode m_mode;

    public: // Renderbuffer Manipulation
        virtual void destroy() = 0;
        virtual void useTexture(GPUTexture* texture) = 0;
        virtual void createOffscreen(int w, int h, int samples) = 0;
        virtual void createTexture(int w, int h, int levels, int samples) = 0;
        virtual void createTextureArray(int w, int h, int layers, int levels, int samples) = 0;
        virtual void createTexture3D(int w, int h, int layers, int levels) = 0;

    public: // Renderbuffer Attributes
        virtual GPUTexturePixelType getPixelType() = 0;
        virtual GPURenderBufferMode getMode() = 0;
        virtual GPUTexture* getTexture() = 0;
        virtual GPUTextureSize getSize() = 0;
        virtual int getWidth() = 0;
        virtual int getHeight() = 0;
        virtual int getDepth() = 0;
        virtual int getLayers() = 0;
        virtual int getLevels() = 0;
        virtual int getSamples() = 0;
};

// ------------------------
// GPU Objects: Framebuffer
// ------------------------

enum class GPUFrameBufferStatus : int {
    FRAMEBUFFER_UNDEFINED,
    FRAMEBUFFER_UNSUPPORTED,
    FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
    FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,
    FRAMEBUFFER_INCOMPLETE_MISSING,
};

typedef struct {
    int layer, level;
} GPUFrameBufferSlice;

class GPUFrameBuffer {
    public: // Framebuffer Attachment
        virtual void destroy() = 0;
        virtual GPUFrameBufferStatus checkAttachments() = 0;
        virtual void attachColor(GPURenderBuffer *target, int index) = 0;
        virtual void attachDepth(GPURenderBuffer *target) = 0;
        virtual void attachStencil(GPURenderBuffer *target) = 0;
        virtual void detachColor(int index) = 0;
        virtual void detachDepth() = 0;
        virtual void detachStencil() = 0;

    public: // Framebuffer Usage
        virtual void setColorIndex(int index) = 0;
        virtual void setColorIndexes(int *list, int count) = 0;
        virtual void setColorSlice(int index, int layer, int level) = 0;
        virtual void setDepthSlice(int layer, int level) = 0;
        virtual void setStencilSlice(int layer, int level) = 0;

    public: // Framebuffer Attributes
        virtual int getColorIndex() = 0;
        virtual int getColorIndexes(int *list, int capacity) = 0;
        virtual GPURenderBuffer* getColorCurrent() = 0;
        virtual GPURenderBuffer* getColor(int index) = 0;
        virtual GPURenderBuffer* getDepth() = 0;
        virtual GPURenderBuffer* getStencil() = 0;
    public: // Framebuffer Attributes: Slice
        virtual GPUFrameBufferSlice getColorSlice(int index) = 0;
        virtual GPUFrameBufferSlice getDepthSlice() = 0;
        virtual GPUFrameBufferSlice getStencilSlice() = 0;
};

#endif // NOGPU_FRAMEBUFFER_H
