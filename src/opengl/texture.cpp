// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "nogpu/opengl_texture.h"
#include "nogpu/opengl_context.h"
#include "glad/glad.h"

// --------------------------
// GPU Enums: Texture Formats
// --------------------------

GLenum toValue(GPUTextureTransferType type) {
    switch (type) {
        case GPUTextureTransferType::TEXTURE_TRANSFER_UNSIGNED_BYTE:
            return GL_UNSIGNED_BYTE;
        case GPUTextureTransferType::TEXTURE_TRANSFER_BYTE:
            return GL_BYTE;
        case GPUTextureTransferType::TEXTURE_TRANSFER_UNSIGNED_SHORT:
            return GL_UNSIGNED_SHORT;
        case GPUTextureTransferType::TEXTURE_TRANSFER_SHORT:
            return GL_SHORT;
        case GPUTextureTransferType::TEXTURE_TRANSFER_UNSIGNED_INT:
            return GL_UNSIGNED_INT;
        case GPUTextureTransferType::TEXTURE_TRANSFER_INT:
            return GL_INT;
        case GPUTextureTransferType::TEXTURE_TRANSFER_FLOAT:
            return GL_FLOAT;
    }

    // Unreachable
    return ~0;
}

GLenum toValue(GPUTexturePixelType type) {
    switch (type) {
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

        // Depth/Stencil Formats
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT16:
            return GL_DEPTH_COMPONENT16;
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT24:
            return GL_DEPTH_COMPONENT24;
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT32:
            return GL_DEPTH_COMPONENT32;
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH24_STENCIL8:
            return GL_DEPTH24_STENCIL8;
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH32F_STENCIL8:
            return GL_DEPTH32F_STENCIL8;

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

        // Simple Compression
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_RED:
            return GL_COMPRESSED_RED;
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_RG:
            return GL_COMPRESSED_RG;
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_RGB:
            return GL_COMPRESSED_RGB;
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_RGBA:
            return GL_COMPRESSED_RGBA;
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_SRGB:
            return GL_COMPRESSED_SRGB;
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_SRGB_ALPHA:
            return GL_COMPRESSED_SRGB_ALPHA;
        // RGTC: Red-Green Texture Compression
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_RED_RGTC1:
            return GL_COMPRESSED_RED_RGTC1;
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_SIGNED_RED_RGTC1:
            return GL_COMPRESSED_SIGNED_RED_RGTC1;
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_RG_RGTC2:
            return GL_COMPRESSED_RG_RGTC2;
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_SIGNED_RG_RGTC2:
            return GL_COMPRESSED_SIGNED_RG_RGTC2;
        // S3TC/DXT compression
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_RGB_S3TC_DXT1:
            return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_RGBA_S3TC_DXT1:
            return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_RGBA_S3TC_DXT3:
            return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_RGBA_S3TC_DXT5:
            return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        // BPTC Float/UNORM compression
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_RGBA_BPTC_UNORM:
            return GL_COMPRESSED_RGBA_BPTC_UNORM_ARB;
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:
            return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB;
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:
            return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB;
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:
            return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB;
    }

    // Unreachable
    return ~0;
}

GLenum toValue(GPUTexturePixelFormat format) {
    switch (format) {
        case GPUTexturePixelFormat::TEXTURE_FORMAT_RED:
            return GL_RED;
        case GPUTexturePixelFormat::TEXTURE_FORMAT_RG:
            return GL_RG;
        case GPUTexturePixelFormat::TEXTURE_FORMAT_RGB:
            return GL_RGB;
        case GPUTexturePixelFormat::TEXTURE_FORMAT_RGBA:
            return GL_RGBA;
        case GPUTexturePixelFormat::TEXTURE_FORMAT_BGR:
            return GL_BGR;
        case GPUTexturePixelFormat::TEXTURE_FORMAT_BGRA:
            return GL_BGRA;
        case GPUTexturePixelFormat::TEXTURE_FORMAT_DEPTH_COMPONENT:
            return GL_DEPTH_COMPONENT;
        case GPUTexturePixelFormat::TEXTURE_FORMAT_DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
    }

    // Unreachable
    return ~0;
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

    // Unreachable
    return ~0;
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

    // Unreachable
    return ~0;
}

GLenum toValue(GPUTextureWrapMode wrap) {
    switch (wrap) {
        case GPUTextureWrapMode::TEXTURE_WRAP_CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case GPUTextureWrapMode::TEXTURE_WRAP_REPEAT:
            return GL_REPEAT;
        case GPUTextureWrapMode::TEXTURE_WRAP_MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
    }

    // Unreachable
    return ~0;
}
