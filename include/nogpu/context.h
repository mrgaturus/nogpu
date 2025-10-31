// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_CONTEXT_H
#define NOGPU_CONTEXT_H
#include "device.h"
#include "buffer.h"
#include "texture.h"
#include "compressed.h"
#include "shader.h"
#include "commands.h"

// -----------
// GPU Context
// -----------

class GPUContext {
    protected: // Context List
        friend GPUDevice;
        GPUContext* m_next;
        GPUContext* m_prev;
        void* m_native;
    protected: static GPUContext* m_current;
    public: virtual void destroy() = 0;

    public: // GPU Buffer Objects
        virtual GPUBuffer* createBuffer() = 0;
        virtual GPUVertexArray* createVertexArray() = 0;
        virtual GPUTextureBuffer* createTextureBuffer(GPUBuffer* buffer, GPUTexturePixelType type) = 0;
    public: // GPU Texture Objects: Standard
        virtual GPUTexture1D* createTexture1D(GPUTexturePixelType type) = 0;
        virtual GPUTexture2D* createTexture2D(GPUTexturePixelType type) = 0;
        virtual GPUTexture3D* createTexture3D(GPUTexturePixelType type) = 0;
        virtual GPUTextureCubemap* createTextureCubemap(GPUTexturePixelType type) = 0;
        virtual GPUTextureCubemapArray* createTextureCubemapArray(GPUTexturePixelType type) = 0;
    public: // GPU Texture Objects: Compressed
        virtual GPUCompressed1D* createCompressed1D(GPUTextureCompressedType type) = 0;
        virtual GPUCompressed2D* createCompressed2D(GPUTextureCompressedType type) = 0;
        virtual GPUCompressed3D* createCompressed3D(GPUTextureCompressedType type) = 0;
        virtual GPUCompressedCubemap* createCompressedCubemap(GPUTextureCompressedType type) = 0;
        virtual GPUCompressedCubemapArray* createCompressedCubemapArray(GPUTextureCompressedType type) = 0;

    public: // GPU Rendering Objects
        virtual GPUFrameBuffer* createFrameBuffer() = 0;
        virtual GPURenderBuffer* createRenderBuffer(GPUTexturePixelType type) = 0;
        virtual GPUShader* createShader(GPUShaderType type, GPUShaderSource data) = 0;
        virtual GPUProgram* createProgram() = 0;
        virtual GPUPipeline* createPipeline() = 0;
        virtual GPUCommands* createCommands() = 0;
    public: // GPU Rendering Surface
        virtual void surfaceSwap() = 0;
        virtual void surfaceResize(int w, int h) = 0;
        virtual GPUDeviceOption surfaceOption() = 0;
        virtual int surfaceSamples() = 0;
        virtual bool surfaceRGBA() = 0;
};

#endif // NOGPU_CONTEXT_H
