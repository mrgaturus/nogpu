// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_TEXTURE_H
#define NOGPU_TEXTURE_H
#include "buffer.h"

// --------------------------
// GPU Enums: Texture Formats
// --------------------------

enum class GPUTextureTransferType : int {
    TEXTURE_TRANSFER_UNSIGNED_BYTE,
    TEXTURE_TRANSFER_BYTE,
    TEXTURE_TRANSFER_UNSIGNED_SHORT,
    TEXTURE_TRANSFER_SHORT,
    TEXTURE_TRANSFER_UNSIGNED_INT,
    TEXTURE_TRANSFER_INT,
    TEXTURE_TRANSFER_FLOAT,
};

enum class GPUTexturePixelType: int {
    TEXTURE_PIXEL_R8,
    TEXTURE_PIXEL_R8_SNORM,
    TEXTURE_PIXEL_R16,
    TEXTURE_PIXEL_R16_SNORM,
    TEXTURE_PIXEL_RG8,
    TEXTURE_PIXEL_RG8_SNORM,
    TEXTURE_PIXEL_RG16,
    TEXTURE_PIXEL_RG16_SNORM,
    TEXTURE_PIXEL_RGB8,
    TEXTURE_PIXEL_RGB8_SNORM,
    TEXTURE_PIXEL_RGB16,
    TEXTURE_PIXEL_RGB16_SNORM,
    TEXTURE_PIXEL_RGBA8,
    TEXTURE_PIXEL_RGBA8_SNORM,
    TEXTURE_PIXEL_RGBA16,
    TEXTURE_PIXEL_RGBA16_SNORM,
    // Depth/Stencil formats
    TEXTURE_PIXEL_DEPTH_COMPONENT16,
    TEXTURE_PIXEL_DEPTH_COMPONENT24,
    TEXTURE_PIXEL_DEPTH_COMPONENT32,
    TEXTURE_PIXEL_DEPTH24_STENCIL8,
    TEXTURE_PIXEL_DEPTH32F_STENCIL8,
    // Floating-point formats
    TEXTURE_PIXEL_R16F,
    TEXTURE_PIXEL_RG16F,
    TEXTURE_PIXEL_RGB16F,
    TEXTURE_PIXEL_RGBA16F,
    TEXTURE_PIXEL_R32F,
    TEXTURE_PIXEL_RG32F,
    TEXTURE_PIXEL_RGB32F,
    TEXTURE_PIXEL_RGBA32F,
    // Integer formats
    TEXTURE_PIXEL_R8I,
    TEXTURE_PIXEL_R8UI,
    TEXTURE_PIXEL_R16I,
    TEXTURE_PIXEL_R16UI,
    TEXTURE_PIXEL_R32I,
    TEXTURE_PIXEL_R32UI,
    TEXTURE_PIXEL_RG8I,
    TEXTURE_PIXEL_RG8UI,
    TEXTURE_PIXEL_RG16I,
    TEXTURE_PIXEL_RG16UI,
    TEXTURE_PIXEL_RG32I,
    TEXTURE_PIXEL_RG32UI,
    TEXTURE_PIXEL_RGB8I,
    TEXTURE_PIXEL_RGB8UI,
    TEXTURE_PIXEL_RGB16I,
    TEXTURE_PIXEL_RGB16UI,
    TEXTURE_PIXEL_RGB32I,
    TEXTURE_PIXEL_RGB32UI,
    TEXTURE_PIXEL_RGBA8I,
    TEXTURE_PIXEL_RGBA8UI,
    TEXTURE_PIXEL_RGBA16I,
    TEXTURE_PIXEL_RGBA16UI,
    TEXTURE_PIXEL_RGBA32I,
    TEXTURE_PIXEL_RGBA32UI,

    // Simple Compression
    TEXTURE_PIXEL_COMPRESSED_RED,
    TEXTURE_PIXEL_COMPRESSED_RG,
    TEXTURE_PIXEL_COMPRESSED_RGB,
    TEXTURE_PIXEL_COMPRESSED_RGBA,
    TEXTURE_PIXEL_COMPRESSED_SRGB,
    TEXTURE_PIXEL_COMPRESSED_SRGB_ALPHA,
    // RGTC: Red-Green Texture Compression
    TEXTURE_PIXEL_COMPRESSED_RED_RGTC1,
    TEXTURE_PIXEL_COMPRESSED_SIGNED_RED_RGTC1,
    TEXTURE_PIXEL_COMPRESSED_RG_RGTC2,
    TEXTURE_PIXEL_COMPRESSED_SIGNED_RG_RGTC2,
    // S3TC/DXT compression
    TEXTURE_PIXEL_COMPRESSED_RGB_S3TC_DXT1,
    TEXTURE_PIXEL_COMPRESSED_RGBA_S3TC_DXT1,
    TEXTURE_PIXEL_COMPRESSED_RGBA_S3TC_DXT3,
    TEXTURE_PIXEL_COMPRESSED_RGBA_S3TC_DXT5,
    // BPTC Float/UNORM compression
    TEXTURE_PIXEL_COMPRESSED_RGBA_BPTC_UNORM,
    TEXTURE_PIXEL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
    TEXTURE_PIXEL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,
    TEXTURE_PIXEL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,
};

enum class GPUTexturePixelFormat : int {
    TEXTURE_FORMAT_RED,
    TEXTURE_FORMAT_RG,
    TEXTURE_FORMAT_RGB,
    TEXTURE_FORMAT_RGBA,
    TEXTURE_FORMAT_BGR,
    TEXTURE_FORMAT_BGRA,
    TEXTURE_FORMAT_DEPTH_COMPONENT,
    TEXTURE_FORMAT_DEPTH_STENCIL
};

// ------------------------
// GPU Enums: Texture Modes
// ------------------------

enum class GPUTextureFilterMode : int {
    TEXTURE_FILTER_NEAREST,
    TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST,
    TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR,
    TEXTURE_FILTER_LINEAR,
    TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST,
    TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR,
};

enum class GPUTextureSwizzleMode : int {
    TEXTURE_SWIZZLE_R,
    TEXTURE_SWIZZLE_G,
    TEXTURE_SWIZZLE_B,
    TEXTURE_SWIZZLE_A,
    TEXTURE_SWIZZLE_ZERO,
    TEXTURE_SWIZZLE_ONE
};

enum class GPUTextureWrapMode : int {
    TEXTURE_WRAP_CLAMP_TO_EDGE,
    TEXTURE_WRAP_REPEAT,
    TEXTURE_WRAP_MIRRORED_REPEAT,
};

// -------------------------
// GPU Objects: Texture Base
// -------------------------

typedef struct {
    GPUTextureSwizzleMode r;
    GPUTextureSwizzleMode g;
    GPUTextureSwizzleMode b;
    GPUTextureSwizzleMode a;
} GPUTextureSwizzle;

typedef struct {
    GPUTextureFilterMode minify;
    GPUTextureFilterMode magnify;
} GPUTextureFilter;

typedef struct {
    GPUTextureWrapMode s;
    GPUTextureWrapMode t;
    GPUTextureWrapMode r;
} GPUTextureWrap;

typedef struct {
    int w, h;
} GPUTextureSize;

class GPUTexture {
    protected:
        GPUTexturePixelType m_pixel_type;
        GPUTexturePixelFormat m_pixel_format;
        GPUTextureTransferType m_transfer_type;
        GPUTextureSwizzle m_swizzle;
        GPUTextureFilter m_filter;
        GPUTextureWrap m_wrap;
        int m_w, m_h;
    // Texture Constructor
    protected: GPUTexture();
    protected: ~GPUTexture();
    public: virtual void destroy() = 0;

    public: // GPU Texture Attributes
        virtual void setTransferType(GPUTextureTransferType type) = 0;
        virtual void setSwizzle(GPUTextureFilter swizzle) = 0;
        virtual void setFilter(GPUTextureFilter filter) = 0;
        virtual void setWrap(GPUTextureFilter wrap) = 0;
        virtual void generateMipmaps() = 0;
        virtual void wait() = 0;

    public: // GPU Texture Attributes
        int getW() { return m_w; }
        int getH() { return m_h; }
        GPUTextureSize getSize() { return (GPUTextureSize) { m_w, m_h }; }
        GPUTexturePixelType getPixelType() { return m_pixel_type; }
        GPUTexturePixelFormat getPixelFormat() { return m_pixel_format; }
        GPUTextureTransferType getTransferType() { return m_transfer_type; }
        GPUTextureSwizzle getPixelSwizzle() { return m_swizzle; }
        GPUTextureFilter getPixelFilter() { return m_filter; }
        GPUTextureWrap getPixelWrap() { return m_wrap; }
};

// ---------------------------
// GPU Objects: Texture Buffer
// ---------------------------

class GPUTextureBuffer : virtual GPUBuffer {
    protected: GPUTextureBuffer();
    protected: ~GPUTextureBuffer();
    // Texture Pixels Manipulation
    public: virtual void setFormat(GPUTexturePixelFormat format);
};

// --------------------
// GPU Objects: Texture
// --------------------

class GPUTexture1D : virtual GPUTexture {
    protected: GPUTexture1D();
    protected: ~GPUTexture1D();

    public: // Texture Pixels Manipulation
        virtual void allocate(int size, int levels) = 0;
        virtual void upload(int x, int size, int level, void* data) = 0;
        virtual void download(int x, int size, int level, void* data) = 0;
        virtual void unpack(int x, int size, int level, GPUBuffer *pbo, int offset) = 0;
        virtual void pack(int x, int size, int level, GPUBuffer *pbo, int offset) = 0;
};

enum class GPUTexture2DMode : int {
    TEXTURE_2D,
    TEXTURE_2D_RECTANGLE,
    TEXTURE_1D_ARRAY,
};

class GPUTexture2D : virtual GPUTexture {
    protected: GPUTexture2D();
    protected: ~GPUTexture2D();

    public: // Texture Pixels Manipulation
        virtual void allocate(int w, int h, int levels) = 0;
        virtual void upload(int x, int y, int w, int h, int level, void* data) = 0;
        virtual void download(int x, int y, int w, int h, int level, void* data) = 0;
        virtual void unpack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int offset) = 0;
        virtual void pack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int offset) = 0;
    public: // Texture Attributes
        virtual void setMode(GPUTexture2DMode mode);
        virtual GPUTexture2DMode getMode();
};

enum class GPUTexture3DMode : int {
    TEXTURE_3D,
    TEXTURE_2D_ARRAY,
};

class GPUTexture3D : virtual GPUTexture {
    protected: GPUTexture3D();
    protected: ~GPUTexture3D();

    public: // Texture Pixels Manipulation
        virtual void allocate(int w, int h, int layers, int levels) = 0;
        virtual void upload(int x, int y, int w, int h, int layer, int level, void* data) = 0;
        virtual void download(int x, int y, int w, int h, int layer, int level, void* data) = 0;
        virtual void unpack(int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int offset) = 0;
        virtual void pack(int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int offset) = 0;
    public: // Texture Attributes
        virtual int getLayers();
        virtual GPUTexture3DMode getMode();
        virtual void setMode(GPUTexture3DMode mode);
};

enum class GPUTextureCubemapSide : int {
    TEXTURE_CUBE_MAP_POSITIVE_X,
    TEXTURE_CUBE_MAP_POSITIVE_Y,
    TEXTURE_CUBE_MAP_POSITIVE_Z,
    TEXTURE_CUBE_MAP_NEGATIVE_X,
    TEXTURE_CUBE_MAP_NEGATIVE_Y,
    TEXTURE_CUBE_MAP_NEGATIVE_Z
};

class GPUTextureCubemap : virtual GPUTexture {
    protected: GPUTextureCubemap();
    protected: ~GPUTextureCubemap();

    public: // Texture Pixels Manipulation
        virtual void allocate(int w, int h, int levels) = 0;
        virtual void upload(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, void* data) = 0;
        virtual void download(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, void* data) = 0;
        virtual void unpack(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, GPUBuffer *pbo, int offset) = 0;
        virtual void pack(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, GPUBuffer *pbo, int offset) = 0;
};

class GPUTextureCubemapArray : virtual GPUTexture {
    protected: GPUTextureCubemapArray();
    protected: ~GPUTextureCubemapArray();

    public: // Texture Pixels Manipulation
        virtual void allocate(int w, int h, int layers, int levels) = 0;
        virtual void upload(int x, int y, int w, int h, int layer, int level, GPUTextureCubemapSide side, void* data) = 0;
        virtual void download(int x, int y, int w, int h, int layer, int level, GPUTextureCubemapSide side, void* data) = 0;
        virtual void unpack(int x, int y, int w, int h, int layer, int level, GPUTextureCubemapSide side, GPUBuffer *pbo, int offset) = 0;
        virtual void pack(int x, int y, int w, int h, int layer, int level, GPUTextureCubemapSide side, GPUBuffer *pbo, int offset) = 0;
    public: // Texture Attributes
        virtual int getLayers();
};

#endif // NOGPU_TEXTURE_H
