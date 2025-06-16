// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H
#include <nogpu/texture.h>
#include "opengl_buffer.h"

GLenum toValue(GPUTextureTransferType type);
GLenum toValue(GPUTexturePixelType type);
GLenum toValue(GPUTexturePixelFormat format);

GLenum toValue(GPUTextureFilterMode swizzle);
GLenum toValue(GPUTextureSwizzleMode filter);
GLenum toValue(GPUTextureWrapMode wrap);

// -------------------------
// OpenGL GPU Texture Buffer
// -------------------------

class GLTextureBuffer : GPUTextureBuffer {
    // Texture Pixels Manipulation
    void setType(GPUTexturePixelType type) override;
    GPUTexturePixelType getType() override;
    GPUBuffer* getBuffer() override;

    public: // Texture Attributes
        GPUTexturePixelType m_pixel_type;
        GLBuffer* m_buffer;
        GLContext* m_ctx;

    protected: // Texture Constructor
        GLTextureBuffer(
            GLContext* ctx,
            GPUTexturePixelType type);
        void destroy() override;
        friend GLContext;
};

// ------------------
// OpenGL GPU Texture
// ------------------

class GLTexture : virtual public GPUTexture {
    // GPU Texture Attributes
    void setTransferType(GPUTextureTransferType type) override;
    void setSwizzle(GPUTextureSwizzle swizzle) override;
    void setFilter(GPUTextureFilter filter) override;
    void setWrap(GPUTextureWrap wrap) override;
    void generateMipmaps() override;
    void syncCPU() override;
    void syncGPU() override;

    public: // Texture Attributes
        GLContext* m_ctx;
        GLenum m_tex_target;
        GLuint m_tex;
        GLsync m_sync;

    protected: // Texture Constructor
        GLTexture(GLContext* ctx);
        void generateTexture();
        void destroy() override;
        friend GLContext;
};

class GLTexture1D : GLTexture, GPUTexture1D {
    // Texture Buffer Manipulation
    void allocate(int size, int levels) override;
    void upload(int x, int size, int level, void* data) override;
    void download(int x, int size, int level, void* data) override;
    void unpack(int x, int size, int level, GPUBuffer *pbo, int offset) override;
    void pack(int x, int size, int level, GPUBuffer *pbo, int offset) override;

    protected: GLTexture1D(
        GLContext* ctx,
        GPUTexturePixelType type,
        GPUTexturePixelFormat format);
        friend GLContext;
};

class GLTexture2D : GLTexture, GPUTexture2D {
    // Texture Buffer Manipulation
    void allocate(int w, int h, int levels) override;
    void upload(int x, int y, int w, int h, int level, void* data) override;
    void download(int x, int y, int w, int h, int level, void* data) override;
    void unpack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int offset) override;
    void pack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int offset) override;
    // Texture Attributes
    GPUTexture2DMode getMode() override;
    void setMode(GPUTexture2DMode mode) override;

    protected: GLTexture2D(
        GLContext* ctx,
        GPUTexturePixelType type,
        GPUTexturePixelFormat format);
        friend GLContext;
};

class GLTexture3D : GLTexture, GPUTexture3D {
    // Texture Buffer Manipulation
    void allocate(int w, int h, int layers, int levels) override;
    void upload(int x, int y, int w, int h, int layer, int level, void* data) override;
    void download(int x, int y, int w, int h, int layer, int level, void* data) override;
    void unpack(int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int offset) override;
    void pack(int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int offset) override;
    // Texture Attributes
    int getLayers() override;
    GPUTexture3DMode getMode() override;
    void setMode(GPUTexture3DMode mode) override;


    protected: GLTexture3D(
        GLContext* ctx,
        GPUTexturePixelType type,
        GPUTexturePixelFormat format);
        friend GLContext;
};

class GLTextureCubemap : GLTexture, GPUTextureCubemap {
    // Texture Buffer Manipulation
    void allocate(int w, int h, int levels) override;
    void upload(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, void* data) override;
    void download(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, void* data) override;
    void unpack(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, GPUBuffer *pbo, int offset) override;
    void pack(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, GPUBuffer *pbo, int offset) override;

    protected: GLTextureCubemap(
        GLContext* ctx,
        GPUTexturePixelType type,
        GPUTexturePixelFormat format);
        friend GLContext;
};

class GLTextureCubemapArray : GLTexture, GPUTextureCubemapArray {
    // Texture Buffer Manipulation
    void allocate(int w, int h, int layers, int levels) override;
    void upload(int x, int y, int w, int h, int layer, int level, GPUTextureCubemapSide side, void* data) override;
    void download(int x, int y, int w, int h, int layer, int level, GPUTextureCubemapSide side, void* data) override;
    void unpack(int x, int y, int w, int h, int layer, int level, GPUTextureCubemapSide side, GPUBuffer *pbo, int offset) override;
    void pack(int x, int y, int w, int h, int layer, int level, GPUTextureCubemapSide side, GPUBuffer *pbo, int offset) override;
    // Texture Attributes
    int getLayers() override;

    protected: GLTextureCubemapArray(
        GLContext* ctx,
        GPUTexturePixelType type,
        GPUTexturePixelFormat format);
        friend GLContext;
};

#endif // OPENGL_TEXTURE_H
