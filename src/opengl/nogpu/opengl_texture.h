// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_TEXTURE_H
#define OPENGL_TEXTURE_H
#include <nogpu/texture.h>
#include "opengl_buffer.h"

// -------------------------
// OpenGL GPU Texture Buffer
// -------------------------

class GLTextureBuffer : GLBuffer, GPUTextureBuffer {
    void setFormat(GPUTexturePixelFormat format) override;

    protected: // Texture Constructor
        GLTextureBuffer(GPUTexturePixelFormat format);
        friend GLContext;
};

// ------------------
// OpenGL GPU Texture
// ------------------

class GLTexture : virtual GPUTexture {
    // GPU Texture Attributes
    void setSwizzle(GPUTextureFilter swizzle) override;
    void setFilter(GPUTextureFilter filter) override;
    void setWrap(GPUTextureFilter wrap) override;
    void generateMipmaps() override;
    void wait() override;

    protected: // Texture Constructor
        GLTexture();
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
        GPUTexturePixelType m_pixel_type,
        GPUTexturePixelFormat m_pixel_format);
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
        GPUTexturePixelType m_pixel_type,
        GPUTexturePixelFormat m_pixel_format);
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
        GPUTexturePixelType m_pixel_type,
        GPUTexturePixelFormat m_pixel_format);
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
        GPUTexturePixelType m_pixel_type,
        GPUTexturePixelFormat m_pixel_format);
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
        GPUTexturePixelType m_pixel_type,
        GPUTexturePixelFormat m_pixel_format);
        friend GLContext;
};

#endif // OPENGL_TEXTURE_H
