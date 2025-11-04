// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "private/texture.h"
#include "private/glad.h"

// ---------------------------
// GPU Enums: Texture Transfer
// ---------------------------

GLenum toValue(GPUTextureTransferSize type) {
    switch (type) {
        case GPUTextureTransferSize::TEXTURE_SIZE_UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        case GPUTextureTransferSize::TEXTURE_SIZE_UNSIGNED_SHORT:
            return GL_UNSIGNED_SHORT;
        case GPUTextureTransferSize::TEXTURE_SIZE_UNSIGNED_INT:
            return GL_UNSIGNED_INT;
        case GPUTextureTransferSize::TEXTURE_SIZE_BYTE:
            return GL_BYTE;
        case GPUTextureTransferSize::TEXTURE_SIZE_SHORT:
            return GL_SHORT;
        case GPUTextureTransferSize::TEXTURE_SIZE_INT:
            return GL_INT;
        case GPUTextureTransferSize::TEXTURE_SIZE_FLOAT:
            return GL_FLOAT;
        case GPUTextureTransferSize::TEXTURE_SIZE_DEPTH24_STENCIL8:
            return GL_UNSIGNED_INT_24_8;
        case GPUTextureTransferSize::TEXTURE_SIZE_COMPRESSED:
            return GL_INVALID_ENUM;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

GLenum toValue(GPUTextureTransferFormat format) {
    switch (format) {
        case GPUTextureTransferFormat::TEXTURE_FORMAT_RED:
            return GL_RED;
        case GPUTextureTransferFormat::TEXTURE_FORMAT_RG:
            return GL_RG;
        case GPUTextureTransferFormat::TEXTURE_FORMAT_RGB:
            return GL_RGB;
        case GPUTextureTransferFormat::TEXTURE_FORMAT_RGBA:
            return GL_RGBA;
        case GPUTextureTransferFormat::TEXTURE_FORMAT_BGR:
            return GL_BGR;
        case GPUTextureTransferFormat::TEXTURE_FORMAT_BGRA:
            return GL_BGRA;
        case GPUTextureTransferFormat::TEXTURE_FORMAT_DEPTH_COMPONENT:
            return GL_DEPTH_COMPONENT;
        case GPUTextureTransferFormat::TEXTURE_FORMAT_DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
        case GPUTextureTransferFormat::TEXTURE_FORMAT_COMPRESSED:
            return GL_INVALID_ENUM;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

// -----------------------
// GPU Enums: Texture Type
// -----------------------

GLenum toValue(GPUTexturePixelType type) {
    switch (type) {
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED:
            return GL_INVALID_ENUM;

        // Unsigned/Signed integer normalized formats
        case GPUTexturePixelType::TEXTURE_PIXEL_R8:
            return GL_R8;
        case GPUTexturePixelType::TEXTURE_PIXEL_R8_SNORM:
            return GL_R8_SNORM;
        case GPUTexturePixelType::TEXTURE_PIXEL_R16:
            return GL_R16;
        case GPUTexturePixelType::TEXTURE_PIXEL_R16_SNORM:
            return GL_R16_SNORM;
        case GPUTexturePixelType::TEXTURE_PIXEL_RG8:
            return GL_RG8;
        case GPUTexturePixelType::TEXTURE_PIXEL_RG8_SNORM:
            return GL_RG8_SNORM;
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16:
            return GL_RG16;
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16_SNORM:
            return GL_RG16_SNORM;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB8:
            return GL_RGB8;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB8_SNORM:
            return GL_RGB8_SNORM;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16:
            return GL_RGB16;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16_SNORM:
            return GL_RGB16_SNORM;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA8:
            return GL_RGBA8;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA8_SNORM:
            return GL_RGBA8_SNORM;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16:
            return GL_RGBA16;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16_SNORM:
            return GL_RGBA16_SNORM;

        // Floating-point formats
        case GPUTexturePixelType::TEXTURE_PIXEL_R16F:
            return GL_R16F;
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16F:
            return GL_RG16F;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16F:
            return GL_RGB16F;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16F:
            return GL_RGBA16F;
        case GPUTexturePixelType::TEXTURE_PIXEL_R32F:
            return GL_R32F;
        case GPUTexturePixelType::TEXTURE_PIXEL_RG32F:
            return GL_RG32F;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB32F:
            return GL_RGB32F;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA32F:
            return GL_RGBA32F;

        // Integer formats
        case GPUTexturePixelType::TEXTURE_PIXEL_R8I:
            return GL_R8I;
        case GPUTexturePixelType::TEXTURE_PIXEL_R8UI:
            return GL_R8UI;
        case GPUTexturePixelType::TEXTURE_PIXEL_R16I:
            return GL_R16I;
        case GPUTexturePixelType::TEXTURE_PIXEL_R16UI:
            return GL_R16UI;
        case GPUTexturePixelType::TEXTURE_PIXEL_R32I:
            return GL_R32I;
        case GPUTexturePixelType::TEXTURE_PIXEL_R32UI:
            return GL_R32UI;

        case GPUTexturePixelType::TEXTURE_PIXEL_RG8I:
            return GL_RG8I;
        case GPUTexturePixelType::TEXTURE_PIXEL_RG8UI:
            return GL_RG8UI;
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16I:
            return GL_RG16I;
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16UI:
            return GL_RG16UI;
        case GPUTexturePixelType::TEXTURE_PIXEL_RG32I:
            return GL_RG32I;
        case GPUTexturePixelType::TEXTURE_PIXEL_RG32UI:
            return GL_RG32UI;

        case GPUTexturePixelType::TEXTURE_PIXEL_RGB8I:
            return GL_RGB8I;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB8UI:
            return GL_RGB8UI;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16I:
            return GL_RGB16I;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16UI:
            return GL_RGB16UI;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB32I:
            return GL_RGB32I;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB32UI:
            return GL_RGB32UI;

        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA8I:
            return GL_RGBA8I;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA8UI:
            return GL_RGBA8UI;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16I:
            return GL_RGBA16I;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16UI:
            return GL_RGBA16UI;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA32I:
            return GL_RGBA32I;
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA32UI:
            return GL_RGBA32UI;

        // Depth/Stencil Formats
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT16:
            return GL_DEPTH_COMPONENT16;
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT24:
            return GL_DEPTH_COMPONENT24;
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT32:
            return GL_DEPTH_COMPONENT32;
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH24_STENCIL8:
            return GL_DEPTH24_STENCIL8;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

GLenum toValue(GPUTextureCompressedType type) {
    switch (type) {
        case GPUTextureCompressedType::TEXTURE_UNCOMPRESSED:
            return GL_INVALID_ENUM;

        // RGTC/Red-Green compression
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_RGTC1_RED:
            return GL_COMPRESSED_RED_RGTC1;
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_RGTC1_RED_SNORM:
            return GL_COMPRESSED_SIGNED_RED_RGTC1;
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_RGTC2_RG:
            return GL_COMPRESSED_RG_RGTC2;
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_RGTC2_RG_SNORM:
            return GL_COMPRESSED_SIGNED_RG_RGTC2;

        // DXTC/S3TC compression
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_DXTC1_RGB:
            return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_DXTC1_RGBA:
            return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_DXTC3_RGBA:
            return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_DXTC5_RGBA:
            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

        // BC7/BPTC compression
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_BC7_RGBA:
            return GL_COMPRESSED_RGBA_BPTC_UNORM_ARB;
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_BC7_sRGBA:
            return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB;
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_BC7_RGB_FLOAT32:
            return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB;
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_BC7_RGB_FLOAT32_SNORM:
            return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB;

        // ETC2 compression
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_ETC2_RGB:
            return GL_COMPRESSED_RGB8_ETC2;
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_ETC2_RGBA:
            return GL_COMPRESSED_RGBA8_ETC2_EAC;
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_ETC2_RGBA_PUNCH:
            return GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_ETC2_sRGB:
            return GL_COMPRESSED_SRGB8_ETC2;
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_ETC2_sRGBA:
            return GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
        case GPUTextureCompressedType::TEXTURE_COMPRESSED_ETC2_sRGBA_PUNCH:   
            return GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;

        // ASTC compression: RGBA
        case GPUTextureCompressedType::TEXTURE_ASTC_RGBA_4x4:
            return GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_RGBA_5x4:
            return GL_COMPRESSED_RGBA_ASTC_5x4_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_RGBA_5x5:
            return GL_COMPRESSED_RGBA_ASTC_5x5_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_RGBA_6x5:
            return GL_COMPRESSED_RGBA_ASTC_6x5_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_RGBA_6x6:
            return GL_COMPRESSED_RGBA_ASTC_6x6_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_RGBA_8x5:
            return GL_COMPRESSED_RGBA_ASTC_8x5_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_RGBA_8x6:
            return GL_COMPRESSED_RGBA_ASTC_8x6_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_RGBA_8x8:
            return GL_COMPRESSED_RGBA_ASTC_8x8_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_RGBA_10x5:
            return GL_COMPRESSED_RGBA_ASTC_10x5_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_RGBA_10x6:
            return GL_COMPRESSED_RGBA_ASTC_10x6_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_RGBA_10x8:
            return GL_COMPRESSED_RGBA_ASTC_10x8_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_RGBA_10x10:
            return GL_COMPRESSED_RGBA_ASTC_10x10_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_RGBA_12x10:
            return GL_COMPRESSED_RGBA_ASTC_12x10_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_RGBA_12x12:
            return GL_COMPRESSED_RGBA_ASTC_12x12_KHR;

        // ASTC compression: SRGBA
        case GPUTextureCompressedType::TEXTURE_ASTC_sRGBA_4x4:
            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_sRGBA_5x4:
            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_sRGBA_5x5:
            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_sRGBA_6x5:
            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_sRGBA_6x6:
            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_sRGBA_8x5:
            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_sRGBA_8x6:
            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_sRGBA_8x8:
            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_sRGBA_10x5:
            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_sRGBA_10x6:
            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_sRGBA_10x8:
            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_sRGBA_10x10:
            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_sRGBA_12x10:
            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR;
        case GPUTextureCompressedType::TEXTURE_ASTC_sRGBA_12x12:
            return GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

// ------------------------
// GPU Enums: Texture Modes
// ------------------------

GLenum toValue(GPUTextureFilterMode filter) {
    switch (filter) {
        case GPUTextureFilterMode::TEXTURE_FILTER_NEAREST:
            return GL_NEAREST;
        case GPUTextureFilterMode::TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
            return GL_NEAREST_MIPMAP_NEAREST;
        case GPUTextureFilterMode::TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
            return GL_NEAREST_MIPMAP_LINEAR;
        case GPUTextureFilterMode::TEXTURE_FILTER_LINEAR:
            return GL_LINEAR;
        case GPUTextureFilterMode::TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
            return GL_LINEAR_MIPMAP_NEAREST;
        case GPUTextureFilterMode::TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

GLenum toValue(GPUTextureSwizzleMode swizzle) {
    switch (swizzle) {
        case GPUTextureSwizzleMode::TEXTURE_SWIZZLE_R:
            return GL_RED;
        case GPUTextureSwizzleMode::TEXTURE_SWIZZLE_G:
            return GL_GREEN;
        case GPUTextureSwizzleMode::TEXTURE_SWIZZLE_B:
            return GL_BLUE;
        case GPUTextureSwizzleMode::TEXTURE_SWIZZLE_A:
            return GL_ALPHA;
        case GPUTextureSwizzleMode::TEXTURE_SWIZZLE_ZERO:
            return GL_ZERO;
        case GPUTextureSwizzleMode::TEXTURE_SWIZZLE_ONE:
            return GL_ONE;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

GLenum toValue(GPUTextureWrapMode wrap) {
    switch (wrap) {
        case GPUTextureWrapMode::TEXTURE_WRAP_CLAMP:
            return GL_CLAMP_TO_EDGE;
        case GPUTextureWrapMode::TEXTURE_WRAP_REPEAT:
            return GL_REPEAT;
        case GPUTextureWrapMode::TEXTURE_WRAP_MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}

// --------------------------
// GPU Enums: Texture Cubemap
// --------------------------

GLenum toValue(GPUTextureCubemapSide side) {
    switch (side) {
        // Positive Cubemap Sides
        case GPUTextureCubemapSide::TEXTURE_CUBEMAP_POSITIVE_X:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        case GPUTextureCubemapSide::TEXTURE_CUBEMAP_POSITIVE_Y:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
        case GPUTextureCubemapSide::TEXTURE_CUBEMAP_POSITIVE_Z:
            return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;

        // Negative Cubemap Sides
        case GPUTextureCubemapSide::TEXTURE_CUBEMAP_NEGATIVE_X:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        case GPUTextureCubemapSide::TEXTURE_CUBEMAP_NEGATIVE_Y:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        case GPUTextureCubemapSide::TEXTURE_CUBEMAP_NEGATIVE_Z:
            return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    }

    // Unreachable Value
    return GL_INVALID_ENUM;
}
