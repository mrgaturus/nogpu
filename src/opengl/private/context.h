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
    GLDriver* m_driver;
    GLDevice* m_device;
    unsigned int m_stole;
    #if defined(__unix__)
        LinuxEGLContext m_egl_context;
    #endif

    // GPU Object Creation
    GPUBuffer* createBuffer() override;
    GPUVertexArray* createVertexArray() override;
    GPUTextureBuffer* createTextureBuffer(GPUBuffer* buffer, GPUTexturePixelType type) override;
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
    GPURenderBuffer* createRenderBuffer(GPUTexturePixelType type) override;
    GPUShader* createShader(GPUShaderType type, GPUShaderSource data) override;
    GPUProgram* createProgram() override;
    GPUPipeline* createPipeline() override;
    GPUCommands* createCommands() override;
    // GPU Rendering Surface: Driver
    void surfaceSwap() override;
    void surfaceResize(int w, int h) override;
    GPUDeviceOption surfaceOption() override;
    int surfaceSamples() override;
    bool surfaceRGBA() override;

    public: void makeCurrent(void* object);
    public: void makeCurrentTexture(void* object);
    protected: // Commands Constructor
        void destroy() override;
        friend GLDriver;
        friend GLDevice;
};

#endif // OPENGL_CONTEXT_H
