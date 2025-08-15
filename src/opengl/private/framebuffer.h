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
class GLFrameBuffer;
class GLRenderBuffer : GPURenderBuffer {
    // Renderbuffer Manipulation
    void useTexture(GPUTexture* texture) override;
    void createTexture(int w, int h, int levels, int samples) override;
    void createTextureArray(int w, int h, int layers, int levels, int samples) override;
    void createOffscreen(int w, int h, int samples) override;
    // Renderbuffer Attributes
    GPUTexture* getTexture() override;
    GPUTextureSize getSize() override;
    int getWidth() override;
    int getHeight() override;
    int getLayers() override;
    int getDepth() override;
    int getSamples() override;

    public:
        GLContext* m_ctx;
        GLTexture* m_target;
        GLuint* m_object;
        GLuint m_tex;
        // Offscreen
        GLint m_samples;
        GLint m_width;
        GLint m_height;

    protected: // Renderbuffer Constructor
        GLRenderBuffer(GLContext* ctx, GPUTexturePixelType type);
        void destroyInternal();
        void destroy() override;
        bool prepareExternal();
        bool prepareInternal();
        friend GLContext;
        friend GLFrameBuffer;
};

// -----------------------
// OpenGL GPU: Framebuffer
// -----------------------

class GLFrameBuffer : GPUFrameBuffer {
    GLContext* m_ctx;

    // Framebuffer Attachment
    GPUFrameBufferStatus checkAttachments() override;
    void attachColor(GPURenderBuffer *target, int index) override;
    void attachDepth(GPURenderBuffer *target) override;
    void attachStencil(GPURenderBuffer *target) override;
    void detachColor(int index) override;
    void detachDepth() override;
    void detachStencil() override;

    // Framebuffer Usage
    void setColorNone() override;
    void setColorCurrent(int index) override;
    void setColorSlice(int layer, int level) override;
    void setDepthSlice(int layer, int level) override;
    void setStencilSlice(int layer, int level) override;

    // Framebuffer Attributes
    int getColorCurrentIndex() override;
    bool getColorCurrentCheck() override;
    GPURenderBuffer* getColorCurrent() override;
    GPURenderBuffer* getColor(int index) override;
    GPURenderBuffer* getDepth() override;
    GPURenderBuffer* getStencil() override;

    protected: // Framebuffer Constructor
        GLFrameBuffer(GLContext* ctx);
        void destroy() override;
        friend GLContext;
};

#endif // OPENGL_FRAMEBUFFER_H
