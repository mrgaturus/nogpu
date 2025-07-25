// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_TEXTURE_H
#define NOGPU_TEXTURE_H
#include "buffer.h"

// --------------------------
// GPU Enums: Texture Formats
// --------------------------

enum class GPUTextureTransferSize : int {
    TEXTURE_SIZE_UNSIGNED_BYTE,
    TEXTURE_SIZE_UNSIGNED_SHORT,
    TEXTURE_SIZE_UNSIGNED_INT,
    TEXTURE_SIZE_BYTE,
    TEXTURE_SIZE_SHORT,
    TEXTURE_SIZE_INT,
    TEXTURE_SIZE_FLOAT,
    TEXTURE_SIZE_DEPTH24_STENCIL8,
    TEXTURE_SIZE_COMPRESSED
};

enum class GPUTextureTransferFormat : int {
    TEXTURE_FORMAT_RED,
    TEXTURE_FORMAT_RG,
    TEXTURE_FORMAT_RGB,
    TEXTURE_FORMAT_RGBA,
    TEXTURE_FORMAT_BGR,
    TEXTURE_FORMAT_BGRA,
    TEXTURE_FORMAT_DEPTH_COMPONENT,
    TEXTURE_FORMAT_DEPTH_STENCIL,
    TEXTURE_FORMAT_COMPRESSED
};

enum class GPUTextureCompressedType : int;
enum class GPUTexturePixelType: int {
    TEXTURE_PIXEL_COMPRESSED,

    // Normalized Formats
    TEXTURE_PIXEL_R8,
    TEXTURE_PIXEL_R16,
    TEXTURE_PIXEL_RG8,
    TEXTURE_PIXEL_RG16,
    TEXTURE_PIXEL_RGB8,
    TEXTURE_PIXEL_RGB16,
    TEXTURE_PIXEL_RGBA8,
    TEXTURE_PIXEL_RGBA16,
    // Signed Normalized Formats
    TEXTURE_PIXEL_R8_SNORM,
    TEXTURE_PIXEL_R16_SNORM,
    TEXTURE_PIXEL_RG8_SNORM,
    TEXTURE_PIXEL_RG16_SNORM,
    TEXTURE_PIXEL_RGB8_SNORM,
    TEXTURE_PIXEL_RGB16_SNORM,
    TEXTURE_PIXEL_RGBA8_SNORM,
    TEXTURE_PIXEL_RGBA16_SNORM,

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

    // Depth/Stencil formats
    TEXTURE_PIXEL_DEPTH_COMPONENT16,
    TEXTURE_PIXEL_DEPTH_COMPONENT24,
    TEXTURE_PIXEL_DEPTH_COMPONENT32,
    TEXTURE_PIXEL_DEPTH24_STENCIL8
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
    TEXTURE_WRAP_CLAMP,
    TEXTURE_WRAP_REPEAT,
    TEXTURE_WRAP_MIRRORED_REPEAT,
};

// ---------------------------
// GPU Objects: Texture Buffer
// ---------------------------

typedef struct {
    int offset;
    int size;
} GPUTextureBufferRange;

class GPUTextureBuffer {
    protected:
        int m_offset;
        int m_size;

    public: // Texture Buffer Setters
        virtual void destroy() = 0;
        virtual void setType(GPUTexturePixelType type) = 0;
        virtual void setBuffer(GPUBuffer* buffer) = 0;
        virtual void setRange(GPUTextureBufferRange range) = 0;
        virtual void clearRange() = 0;

    public: // Texture Buffer Getters
        virtual GPUTexturePixelType getType() = 0;
        virtual GPUBuffer* getBuffer() = 0;
        GPUTextureBufferRange getRange() {
            return (GPUTextureBufferRange) {m_offset, m_size};
        };
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
    int width;
    int height;
} GPUTextureSize;

class GPUTexture {
    protected:
        GPUTexturePixelType m_pixel_type;
        GPUTextureCompressedType m_compressed_type;
        GPUTextureTransferSize m_transfer_size;
        GPUTextureTransferFormat m_transfer_format;
        GPUTextureSwizzle m_swizzle;
        GPUTextureFilter m_filter;
        GPUTextureWrap m_wrap;
        // Texture Dimensions
        int m_levels;
        int m_width;
        int m_height;
        int m_depth;

    public: // GPU Texture Attributes
        virtual void destroy() = 0;
        virtual void setTransferSize(GPUTextureTransferSize type) = 0;
        virtual void setTransferFormat(GPUTextureTransferFormat format) = 0;
        virtual void setSwizzle(GPUTextureSwizzle swizzle) = 0;
        virtual void setFilter(GPUTextureFilter filter) = 0;
        virtual void setWrap(GPUTextureWrap wrap) = 0;
        virtual void generateMipmaps() = 0;

    public: // GPU Texture Fences
        virtual void syncCPU() = 0;
        virtual void syncGPU() = 0;
        virtual void syncEnable() = 0;
        virtual void syncDisable() = 0;

    public: // GPU Texture Attributes: Size
        int getLevels() { return m_levels; }
        int getWidth() { return m_width; }
        int getHeight() { return m_height; }
        int getDepth() { return m_depth; }
        int getLayers() { return m_depth; }

    public: // GPU Texture Attributes
        int getTransferBytesPerPixel();
        GPUTextureSize getSize(int level);
        GPUTexturePixelType getPixelType() { return m_pixel_type; }
        GPUTextureTransferSize getTransferSize() { return m_transfer_size; }
        GPUTextureTransferFormat getTransferFormat() { return m_transfer_format; }
        GPUTextureCompressedType getCompressedType() { return m_compressed_type; }
        GPUTextureSwizzle getPixelSwizzle() { return m_swizzle; }
        GPUTextureFilter getPixelFilter() { return m_filter; }
        GPUTextureWrap getPixelWrap() { return m_wrap; }
};

// --------------------
// GPU Objects: Texture
// --------------------

class GPUTexture1D : public virtual GPUTexture {
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

class GPUTexture2D : public virtual GPUTexture {
    public: // Texture Pixels Manipulation
        virtual GPUTexture2DMode getMode() = 0;
        virtual void allocate(GPUTexture2DMode mode, int w, int h, int levels) = 0;
        virtual void upload(int x, int y, int w, int h, int level, void* data) = 0;
        virtual void download(int x, int y, int w, int h, int level, void* data) = 0;
        virtual void unpack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int offset) = 0;
        virtual void pack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int offset) = 0;
};

enum class GPUTexture3DMode : int {
    TEXTURE_3D,
    TEXTURE_2D_ARRAY,
};

class GPUTexture3D : public virtual GPUTexture {
    public: // Texture Pixels Manipulation
        virtual GPUTexture3DMode getMode() = 0;
        virtual void allocate(GPUTexture3DMode mode, int w, int h, int depth, int levels) = 0;
        virtual void upload(int x, int y, int z, int w, int h, int depth, int level, void* data) = 0;
        virtual void download(int x, int y, int z, int w, int h, int depth, int level, void* data) = 0;
        virtual void unpack(int x, int y, int z, int w, int h, int depth, int level, GPUBuffer *pbo, int offset) = 0;
        virtual void pack(int x, int y, int z, int w, int h, int depth, int level, GPUBuffer *pbo, int offset) = 0;
};

enum class GPUTextureCubemapSide : int {
    TEXTURE_CUBEMAP_POSITIVE_X,
    TEXTURE_CUBEMAP_POSITIVE_Y,
    TEXTURE_CUBEMAP_POSITIVE_Z,
    TEXTURE_CUBEMAP_NEGATIVE_X,
    TEXTURE_CUBEMAP_NEGATIVE_Y,
    TEXTURE_CUBEMAP_NEGATIVE_Z
};

class GPUTextureCubemap : public virtual GPUTexture {
    public: // Texture Pixels Manipulation
        virtual void allocate(int w, int h, int levels) = 0;
        virtual void upload(GPUTextureCubemapSide side, int x, int y, int w, int h, int level, void* data) = 0;
        virtual void download(GPUTextureCubemapSide side, int x, int y, int w, int h, int level, void* data) = 0;
        virtual void unpack(GPUTextureCubemapSide side, int x, int y, int w, int h, int level, GPUBuffer *pbo, int offset) = 0;
        virtual void pack(GPUTextureCubemapSide side, int x, int y, int w, int h, int level, GPUBuffer *pbo, int offset) = 0;
};

class GPUTextureCubemapArray : public virtual GPUTexture {
    public: // Texture Pixels Manipulation
        virtual void allocate(int w, int h, int layers, int levels) = 0;
        virtual void upload(GPUTextureCubemapSide side, int x, int y, int w, int h, int layer, int level, void* data) = 0;
        virtual void download(GPUTextureCubemapSide side, int x, int y, int w, int h, int layer, int level, void* data) = 0;
        virtual void unpack(GPUTextureCubemapSide side, int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int offset) = 0;
        virtual void pack(GPUTextureCubemapSide side, int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int offset) = 0;
};

#endif // NOGPU_TEXTURE_H
