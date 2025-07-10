// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_CONTEXT_H
#define OPENGL_CONTEXT_H
#include <nogpu/context.h>
#include "driver.h"

// ------------------
// OpenGL GPU Context
// ------------------

class GLContext : GPUContext {
    #if defined(__unix__)
        LinuxEGLContext m_gtx;
        GLDriver* m_driver;
    #endif

    // GPU Object Creation
    GPUBuffer* createBuffer() override;
    GPUVertexArray* createVertexArray() override;
    GPUTextureBuffer* createTextureBuffer(GPUBuffer* buffer, GPUTexturePixelType type) override;
    GPURenderBuffer* createRenderBuffer(GPUTexturePixelType type) override;

    // GPU Texture Objects: Standard
    GPUTexture1D* createTexture1D(GPUTexturePixelType type) override;
    GPUTexture2D* createTexture2D(GPUTexturePixelType type) override;
    GPUTexture3D* createTexture3D(GPUTexturePixelType type) override;
    GPUTextureCubemap* createTextureCubemap(GPUTexturePixelType type) override;
    GPUTextureCubemapArray* createTextureCubemapArray(GPUTexturePixelType type) override;
    // GPU Texture Objects: Compressed
    GPUCompressed1D* createCompressed1D(GPUTextureCompressedType type) override;
    GPUCompressed2D* createCompressed2D(GPUTextureCompressedType type) override;
    GPUCompressed3D* createCompressed3D(GPUTextureCompressedType type) override;
    GPUCompressedCubemap* createCompressedCubemap(GPUTextureCompressedType type) override;
    GPUCompressedCubemapArray* createCompressedCubemapArray(GPUTextureCompressedType type) override;

    // GPU Rendering Objects
    GPUFrameBuffer* createFrameBuffer() override;
    GPUProgram* createProgram() override;
    GPUShader* createShader(GPUShaderType type, char* buffer, int size) override;
    GPUPipeline* createPipeline(GPUProgram* program) override;

    // GPU Object Commands
    GPUCommands* createCommands() override;
    void submit(GPUCommands* commands) override;
    void recreateSurface(int w, int h) override;
    void swapSurface() override;

    public: void gl__makeCurrent();
    protected: // Commands Constructor
        bool isTransparent() override;
        void destroy() override;
        friend GLDriver;
};

#endif // OPENGL_CONTEXT_H
