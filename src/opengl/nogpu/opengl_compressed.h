// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_COMPRESSED_H
#define OPENGL_COMPRESSED_H
#include <nogpu/compressed.h>
#include "opengl_texture.h"

GLenum toValue(GPUTextureCompressedType type);

class GLCompressed1D : GLTexture, GPUCompressed1D {
    // Texture Buffer Manipulation
    void allocate(int size, int levels) override;
    void upload(int x, int size, int level, void* data, int bytes) override;
    void unpack(int x, int size, int level, GPUBuffer *pbo, int bytes, int offset) override;

    protected: GLCompressed1D(
        GLContext* ctx,
        GPUTextureCompressedType type,
        GPUTexturePixelFormat format);
        friend GLContext;
};

class GLCompressed2D : GLTexture, GPUCompressed2D {
    GPUTexture2DMode getMode() override;
    void setMode(GPUTexture2DMode mode);

    // Texture Buffer Manipulation
    void allocate(GPUTexture2DMode mode, int w, int h, int levels) override;
    void upload(int x, int y, int w, int h, int level, void* data, int bytes) override;
    void unpack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int bytes, int offset) override;

    protected: GLCompressed2D(
        GLContext* ctx,
        GPUTextureCompressedType type,
        GPUTexturePixelFormat format);
        friend GLContext;
};

class GLCompressed3D : GLTexture, GPUCompressed3D {
    GPUTexture3DMode getMode() override;
    void setMode(GPUTexture3DMode mode);

    // Texture Buffer Manipulation
    void allocate(GPUTexture3DMode mode, int w, int h, int depth, int levels) override;
    void upload(int x, int y, int z, int w, int h, int depth, int level, void* data, int bytes) override;
    void unpack(int x, int y, int z, int w, int h, int depth, int level, GPUBuffer *pbo, int bytes, int offset) override;

    protected: GLCompressed3D(
        GLContext* ctx,
        GPUTextureCompressedType type,
        GPUTexturePixelFormat format);
        friend GLContext;
};

class GLCompressedCubemap : GLTexture, GPUCompressedCubemap {
    // Texture Buffer Manipulation
    void allocate(int w, int h, int levels) override;
    void upload(GPUTextureCubemapSide side, int x, int y, int w, int h, int level, void* data, int bytes) override;
    void unpack(GPUTextureCubemapSide side, int x, int y, int w, int h, int level, GPUBuffer *pbo, int bytes, int offset) override;

    protected: GLCompressedCubemap(
        GLContext* ctx,
        GPUTextureCompressedType type,
        GPUTexturePixelFormat format);
        friend GLContext;
};

class GLCompressedCubemapArray : GLTexture, GPUCompressedCubemapArray {
    // Texture Buffer Manipulation
    void allocate(int w, int h, int layers, int levels) override;
    void upload(GPUTextureCubemapSide side, int x, int y, int w, int h, int layer, int level, void* data, int bytes) override;
    void unpack(GPUTextureCubemapSide side, int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int bytes, int offset) override;

    protected: GLCompressedCubemapArray(
        GLContext* ctx,
        GPUTextureCompressedType type,
        GPUTexturePixelFormat format);
        friend GLContext;
};

#endif // OPENGL_COMPRESSED_H