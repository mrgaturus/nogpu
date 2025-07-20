// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_FRAMEBUFFER_H
#define OPENGL_FRAMEBUFFER_H
#include <nogpu/framebuffer.h>
#include "texture.h"

// ------------------------------
// OpenGL GPU: Framebuffer Target
// ------------------------------

class GLContext;
class GLRenderBuffer : GPURenderBuffer {
    // Renderbuffer Manipulation
    void useTexture(GPUTexture* texture) override;
    void createTexture(int w, int h, int samples) override;
    void createTextureArray(int w, int h, int samples, int layers) override;
    void createOffscreen(int w, int h, int samples) override;
    // Renderbuffer Attributes
    GPUTexture* getTexture() override;
    GPUTextureSize getSize() override;
    int getSamples() override;
    int getWidth() override;
    int getHeight() override;
    int getLayers() override;

    public:
        GLContext* m_ctx;
        GLTexture* m_texture;
        GLuint* m_object;

    protected: // Renderbuffer Constructor
        GLRenderBuffer(GLContext* ctx, GPUTexturePixelType type);
        void destroyInternal();
        void destroy() override;
        friend GLContext;
};

// -----------------------
// OpenGL GPU: Framebuffer
// -----------------------

class GLFrameBuffer : GPUFrameBuffer {
    GLContext* m_ctx;

    // Framebuffer Attachment
    void attachColor(GPURenderBuffer *target, int index) override;
    void attachStencil(GPURenderBuffer *target) override;
    void attachDepth(GPURenderBuffer *target) override;
    // Framebuffer Detachment
    void detachColor() override;
    void detachStencil() override;
    void detachDepth() override;

    // Framebuffer Attributes
    GPUFrameBufferStatus checkStatus() override;
    GPURenderBuffer* getColor(int index) override;
    GPURenderBuffer* getStencil() override;
    GPURenderBuffer* getDepth() override;

    protected: // Framebuffer Constructor
        GLFrameBuffer(GLContext* ctx);
        void destroy() override;
        friend GLContext;
};

#endif // OPENGL_FRAMEBUFFER_H
