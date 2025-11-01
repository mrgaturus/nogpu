// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "nogpu_private.h"
#include <nogpu/texture.h>

bool canTextureBuffer(GPUTexturePixelType type) {
    switch (type) {
        case GPUTexturePixelType::TEXTURE_PIXEL_R8:
        case GPUTexturePixelType::TEXTURE_PIXEL_R16:
        case GPUTexturePixelType::TEXTURE_PIXEL_R16F:
        case GPUTexturePixelType::TEXTURE_PIXEL_R32F:
        case GPUTexturePixelType::TEXTURE_PIXEL_R8I:
        case GPUTexturePixelType::TEXTURE_PIXEL_R16I:
        case GPUTexturePixelType::TEXTURE_PIXEL_R32I:
        case GPUTexturePixelType::TEXTURE_PIXEL_R8UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_R16UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_R32UI:
            return true;

        case GPUTexturePixelType::TEXTURE_PIXEL_RG8:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG32F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG8I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG32I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG8UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG32UI:
            return true;

        case GPUTexturePixelType::TEXTURE_PIXEL_RGB8:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB32F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB8I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB32I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB8UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB32UI:
            return true;

        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA8:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA32F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA8I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA32I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA8UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA32UI:
            return true;

        default: // Invalid Type
            return false;
    }
}

bool canTransferChange(GPUTexturePixelType type) {
    switch (type) {
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT16:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT24:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT32:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH24_STENCIL8:
            return false;
        default: return true;
    }
}

// -----------------------------------
// Texture Pixel Types: Transfer Modes
// -----------------------------------

GPUTextureTransferFormat computeTransferFormat(GPUTexturePixelType type) {
    switch (type) {
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED:
            return GPUTextureTransferFormat::TEXTURE_FORMAT_COMPRESSED;

        case GPUTexturePixelType::TEXTURE_PIXEL_R8:
        case GPUTexturePixelType::TEXTURE_PIXEL_R16:
        case GPUTexturePixelType::TEXTURE_PIXEL_R8_SNORM:
        case GPUTexturePixelType::TEXTURE_PIXEL_R16_SNORM:
        case GPUTexturePixelType::TEXTURE_PIXEL_R16F:
        case GPUTexturePixelType::TEXTURE_PIXEL_R32F:
        case GPUTexturePixelType::TEXTURE_PIXEL_R8I:
        case GPUTexturePixelType::TEXTURE_PIXEL_R8UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_R16I:
        case GPUTexturePixelType::TEXTURE_PIXEL_R16UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_R32I:
        case GPUTexturePixelType::TEXTURE_PIXEL_R32UI:
            return GPUTextureTransferFormat::TEXTURE_FORMAT_RED;

        case GPUTexturePixelType::TEXTURE_PIXEL_RG8:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG8_SNORM:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16_SNORM:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG32F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG8I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG8UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG32I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG32UI:
            return GPUTextureTransferFormat::TEXTURE_FORMAT_RG;

        case GPUTexturePixelType::TEXTURE_PIXEL_RGB8:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB8_SNORM:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16_SNORM:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB32F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB8I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB8UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB32I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB32UI:
            return GPUTextureTransferFormat::TEXTURE_FORMAT_RGB;

        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA8:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA8_SNORM:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16_SNORM:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA32F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA8I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA8UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA32I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA32UI:
            return GPUTextureTransferFormat::TEXTURE_FORMAT_RGBA;

        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT16:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT24:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT32:
            return GPUTextureTransferFormat::TEXTURE_FORMAT_DEPTH_COMPONENT;
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH24_STENCIL8:
            return GPUTextureTransferFormat::TEXTURE_FORMAT_DEPTH_STENCIL; 
    }

    // Fallback to a most used format
    return GPUTextureTransferFormat::TEXTURE_FORMAT_RGBA;
}

GPUTextureTransferSize computeTransferSize(GPUTexturePixelType type) {
    switch (type) {
        case GPUTexturePixelType::TEXTURE_PIXEL_COMPRESSED:
            return GPUTextureTransferSize::TEXTURE_SIZE_COMPRESSED;

        case GPUTexturePixelType::TEXTURE_PIXEL_R8:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG8:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB8:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA8:
        case GPUTexturePixelType::TEXTURE_PIXEL_R8UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG8UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB8UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA8UI:
            return GPUTextureTransferSize::TEXTURE_SIZE_UNSIGNED_BYTE;

        case GPUTexturePixelType::TEXTURE_PIXEL_R16:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16:
        case GPUTexturePixelType::TEXTURE_PIXEL_R16UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT16:
            return GPUTextureTransferSize::TEXTURE_SIZE_UNSIGNED_SHORT;

        case GPUTexturePixelType::TEXTURE_PIXEL_R32UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG32UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB32UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA32UI:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT24:
        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH_COMPONENT32:
            return GPUTextureTransferSize::TEXTURE_SIZE_UNSIGNED_INT;

        case GPUTexturePixelType::TEXTURE_PIXEL_R8I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG8I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB8I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA8I:
        case GPUTexturePixelType::TEXTURE_PIXEL_R8_SNORM:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG8_SNORM:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB8_SNORM:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA8_SNORM:
            return GPUTextureTransferSize::TEXTURE_SIZE_BYTE;

        case GPUTexturePixelType::TEXTURE_PIXEL_R16I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16I:
        case GPUTexturePixelType::TEXTURE_PIXEL_R16_SNORM:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16_SNORM:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16_SNORM:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16_SNORM:
            return GPUTextureTransferSize::TEXTURE_SIZE_SHORT;

        case GPUTexturePixelType::TEXTURE_PIXEL_R32I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG32I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB32I:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA32I:
            return GPUTextureTransferSize::TEXTURE_SIZE_INT;

        case GPUTexturePixelType::TEXTURE_PIXEL_R16F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG16F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB16F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA16F:
        case GPUTexturePixelType::TEXTURE_PIXEL_R32F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RG32F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGB32F:
        case GPUTexturePixelType::TEXTURE_PIXEL_RGBA32F:
            return GPUTextureTransferSize::TEXTURE_SIZE_FLOAT;

        case GPUTexturePixelType::TEXTURE_PIXEL_DEPTH24_STENCIL8:
            return GPUTextureTransferSize::TEXTURE_SIZE_DEPTH24_STENCIL8;
    }

    // Fallback to a most used size
    return GPUTextureTransferSize::TEXTURE_SIZE_UNSIGNED_BYTE;
}

// ---------------------------------------------
// Texture Pixel Types: Transfer Bytes per Pixel
// ---------------------------------------------

int computeTransferBytesPerPixel(GPUTextureTransferFormat format, GPUTextureTransferSize size) {
    int bytes = 0;

    switch (size) {
        case GPUTextureTransferSize::TEXTURE_SIZE_COMPRESSED:
        case GPUTextureTransferSize::TEXTURE_SIZE_UNSIGNED_BYTE:
        case GPUTextureTransferSize::TEXTURE_SIZE_BYTE:
            bytes = (int) sizeof(unsigned char);
        case GPUTextureTransferSize::TEXTURE_SIZE_UNSIGNED_SHORT:
        case GPUTextureTransferSize::TEXTURE_SIZE_SHORT:
            bytes = (int) sizeof(unsigned short);
        case GPUTextureTransferSize::TEXTURE_SIZE_FLOAT:
        case GPUTextureTransferSize::TEXTURE_SIZE_DEPTH24_STENCIL8:
        case GPUTextureTransferSize::TEXTURE_SIZE_UNSIGNED_INT:
        case GPUTextureTransferSize::TEXTURE_SIZE_INT:
            bytes = (int) sizeof(unsigned int);
    }

    switch (format) {
        case GPUTextureTransferFormat::TEXTURE_FORMAT_COMPRESSED:
        case GPUTextureTransferFormat::TEXTURE_FORMAT_DEPTH_COMPONENT:
        case GPUTextureTransferFormat::TEXTURE_FORMAT_DEPTH_STENCIL:
        case GPUTextureTransferFormat::TEXTURE_FORMAT_RED:
            bytes *= 1;
        case GPUTextureTransferFormat::TEXTURE_FORMAT_RG:
            bytes *= 2;
        case GPUTextureTransferFormat::TEXTURE_FORMAT_RGB:
        case GPUTextureTransferFormat::TEXTURE_FORMAT_BGR:
            bytes *= 3;
        case GPUTextureTransferFormat::TEXTURE_FORMAT_RGBA:
        case GPUTextureTransferFormat::TEXTURE_FORMAT_BGRA:
            bytes *= 4;
    }

    // Return Transfer Bytes
    return bytes;
}

int GPUTexture::getTransferBytesPerPixel() {
    return computeTransferBytesPerPixel(m_transfer_format, m_transfer_size);
}

// --------------------
// Texture Object: Size
// --------------------

GPUTextureSize GPUTexture::getSize(int level) {
    GPUTextureSize size = {0, 0};
    if (level <= 0 || level >= m_levels)
        return size;

    // Calculate Current Levels
    size.width = m_width >> level;
    size.height = m_height >> level;
    if (size.width <= 0) size.width = 1;
    if (size.height <= 0) size.height = 1;

    // Return Level Size
    return size;
}
