// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_FRAMEBUFFER_H
#define OPENGL_FRAMEBUFFER_H
#include <nogpu/framebuffer.h>
#include <nogpu_map.h>
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
    GPUTexturePixelType getPixelType() override;
    GPURenderBufferMode getMode() override;
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
        void updateExternal();
        void prepareInternal();
        friend GLContext;
        friend GLFrameBuffer;
};

// -----------------------
// OpenGL GPU: Framebuffer
// -----------------------

class GLFrameBuffer : GPUFrameBuffer {
    GLContext* m_ctx;
    typedef struct {
        GLRenderBuffer* target;
        GPUFrameBufferSlice slice;
        GLuint tex_index;
        GLuint tex_cache;
    } GLRenderLink;

    typedef struct {
        int count, capacity;
        // List Pointers
        GLRenderLink **links;
        GLuint *indexes;
    } GLRenderIndexes;

    private: // Framebuffer Internals
        GPUHashmap<GLRenderLink> m_colors;
        GLRenderIndexes m_colors_index;
        GLRenderLink m_depth;
        GLRenderLink m_stencil;
        GLRenderLink *m_color;
        GLuint m_color_index;
        GLuint m_fbo;

    // Framebuffer Attachment
    static void updateAttachment(GLenum attachment, GLRenderLink* link);
    GPUFrameBufferStatus checkAttachments() override;
    void attachColor(GPURenderBuffer *target, int index) override;
    void attachDepth(GPURenderBuffer *target) override;
    void attachStencil(GPURenderBuffer *target) override;
    void detachColor(int index) override;
    void detachDepth() override;
    void detachStencil() override;

    // Framebuffer Usage
    void setColorIndex(int index) override;
    void setColorIndexes(int *list, int count) override;
    void setColorSlice(int index, int layer, int level) override;
    void setDepthSlice(int layer, int level) override;
    void setStencilSlice(int layer, int level) override;

    // Framebuffer Attributes
    int getColorIndex() override;
    int getColorIndexes(int *list, int capacity) override;
    GPURenderBuffer* getColorCurrent() override;
    GPURenderBuffer* getColor(int index) override;
    GPURenderBuffer* getDepth() override;
    GPURenderBuffer* getStencil() override;
    // Framebuffer Attributes: Slice
    GPUFrameBufferSlice getColorSlice(int index) override;
    GPUFrameBufferSlice getDepthSlice() override;
    GPUFrameBufferSlice getStencilSlice() override;

    protected: // Framebuffer Constructor
        GLFrameBuffer(GLContext* ctx);
        void destroy() override;
        void reserveIndexes(int count);
        void updateIndexes();
        friend GLContext;
};

#endif // OPENGL_FRAMEBUFFER_H
