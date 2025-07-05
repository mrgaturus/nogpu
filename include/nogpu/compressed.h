// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_COMPRESSED_H
#define NOGPU_COMPRESSED_H
#include "texture.h"

enum class GPUCompressedPixelType : int {
    // Simple compression
    TEXTURE_COMPRESSED_SIMPLE_RED,
    TEXTURE_COMPRESSED_SIMPLE_RG,
    TEXTURE_COMPRESSED_SIMPLE_RGB,
    TEXTURE_COMPRESSED_SIMPLE_RGBA,
    TEXTURE_COMPRESSED_SIMPLE_sRGB,
    TEXTURE_COMPRESSED_SIMPLE_sRGBA,

    // RGTC/Red-Green compression
    TEXTURE_COMPRESSED_RGTC1_RED,
    TEXTURE_COMPRESSED_RGTC1_RED_SNORM,
    TEXTURE_COMPRESSED_RGTC2_RG,
    TEXTURE_COMPRESSED_RGTC2_RG_SNORM,

    // DXTC/S3TC compression
    TEXTURE_COMPRESSED_DXTC1_RGB,
    TEXTURE_COMPRESSED_DXTC1_RGBA,
    TEXTURE_COMPRESSED_DXTC3_RGBA,
    TEXTURE_COMPRESSED_DXTC5_RGBA,

    // BC7/BPTC compression
    TEXTURE_COMPRESSED_BC7_RGBA,
    TEXTURE_COMPRESSED_BC7_sRGBA,
    TEXTURE_COMPRESSED_BC7_RGB_FLOAT32,
    TEXTURE_COMPRESSED_BC7_RGB_FLOAT32_SNORM,

    // ETC compression
    TEXTURE_COMPRESSED_ETC1_RGB,
    TEXTURE_COMPRESSED_ETC2_RGB,
    TEXTURE_COMPRESSED_ETC2_RGBA,
    TEXTURE_COMPRESSED_ETC2_sRGBA,

    // ASTC compression: RGBA
    TEXTURE_ASTC_RGBA_4x4,
    TEXTURE_ASTC_RGBA_5x4,
    TEXTURE_ASTC_RGBA_5x5,
    TEXTURE_ASTC_RGBA_6x5,
    TEXTURE_ASTC_RGBA_6x6,
    TEXTURE_ASTC_RGBA_8x5,
    TEXTURE_ASTC_RGBA_8x6,
    TEXTURE_ASTC_RGBA_8x8,
    TEXTURE_ASTC_RGBA_10x5,
    TEXTURE_ASTC_RGBA_10x6,
    TEXTURE_ASTC_RGBA_10x8,
    TEXTURE_ASTC_RGBA_10x10,
    TEXTURE_ASTC_RGBA_12x10,
    TEXTURE_ASTC_RGBA_12x12,
    // ASTC compression: sRGBA
    TEXTURE_ASTC_sRGBA_4x4,
    TEXTURE_ASTC_sRGBA_5x4,
    TEXTURE_ASTC_sRGBA_5x5,
    TEXTURE_ASTC_sRGBA_6x5,
    TEXTURE_ASTC_sRGBA_6x6,
    TEXTURE_ASTC_sRGBA_8x5,
    TEXTURE_ASTC_sRGBA_8x6,
    TEXTURE_ASTC_sRGBA_8x8,
    TEXTURE_ASTC_sRGBA_10x5,
    TEXTURE_ASTC_sRGBA_10x6,
    TEXTURE_ASTC_sRGBA_10x8,
    TEXTURE_ASTC_sRGBA_10x10,
    TEXTURE_ASTC_sRGBA_12x10,
    TEXTURE_ASTC_sRGBA_12x12
};

class GPUCompressed1D : virtual GPUTexture {
    protected: GPUCompressed1D();
    protected: ~GPUCompressed1D();

    public: // Texture Pixels Manipulation
        virtual void allocate(int size, int levels) = 0;
        virtual void upload(int x, int size, int level, void* data) = 0;
        virtual void unpack(int x, int size, int level, GPUBuffer *pbo, int offset) = 0;
};

class GPUCompressed2D : virtual GPUTexture {
    protected: GPUCompressed2D();
    protected: ~GPUCompressed2D();

    public: // Texture Pixels Manipulation
        virtual GPUTexture2DMode getMode();
        virtual void allocate(GPUTexture2DMode mode, int w, int h, int levels) = 0;
        virtual void upload(int x, int y, int w, int h, int level, void* data) = 0;
        virtual void unpack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int offset) = 0;
};

class GPUCompressed3D : virtual GPUTexture {
    protected: GPUCompressed3D();
    protected: ~GPUCompressed3D();

    public: // Texture Pixels Manipulation
        virtual GPUTexture3DMode getMode();
        virtual void allocate(GPUTexture3DMode mode, int w, int h, int depth, int levels) = 0;
        virtual void upload(int x, int y, int z, int w, int h, int depth, int level, void* data) = 0;
        virtual void unpack(int x, int y, int z, int w, int h, int depth, int level, GPUBuffer *pbo, int offset) = 0;
};

class GPUCompressedCubemap : virtual GPUTexture {
    protected: GPUCompressedCubemap();
    protected: ~GPUCompressedCubemap();

    public: // Texture Pixels Manipulation
        virtual void allocate(int w, int h, int levels) = 0;
        virtual void upload(GPUTextureCubemapSide side, int x, int y, int w, int h, int level, void* data) = 0;
        virtual void unpack(GPUTextureCubemapSide side, int x, int y, int w, int h, int level, GPUBuffer *pbo, int offset) = 0;
};

class GPUCompressedCubemapArray : virtual GPUTexture {
    protected: GPUCompressedCubemapArray();
    protected: ~GPUCompressedCubemapArray();

    public: // Texture Pixels Manipulation
        virtual void allocate(int w, int h, int layers, int levels) = 0;
        virtual void upload(GPUTextureCubemapSide side, int x, int y, int w, int h, int layer, int level, void* data) = 0;
        virtual void unpack(GPUTextureCubemapSide side, int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int offset) = 0;
};

#endif // NOGPU_COMPRESSED_H