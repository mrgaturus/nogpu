// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "private/buffer.h"
#include "private/context.h"
#include "private/texture.h"
#include "private/compressed.h"
#include "private/framebuffer.h"
#include "private/glad.h"

bool GLContext::isTransparent() {
    return false;
}

// -------------------
// GPU Object Creation
// -------------------

GPUBuffer* GLContext::createBuffer() {
    return new GLBuffer(this);
}

GPUVertexArray* GLContext::createVertexArray() {
    return new GLVertexArray(this);
};

GPUTextureBuffer* GLContext::createTextureBuffer(GPUBuffer* buffer, GPUTexturePixelType type) {
    return new GLTextureBuffer(this, static_cast<GLBuffer*>(buffer), type);
}

GPURenderBuffer* GLContext::createRenderBuffer(GPUTexturePixelType type) {
    return new GLRenderBuffer(this, type);
}

// -------------------------------------
// GPU Object Creation: Texture Standard
// -------------------------------------

GPUTexture1D* GLContext::createTexture1D(GPUTexturePixelType type) {
    return new GLTexture1D(this, type);
}

GPUTexture2D* GLContext::createTexture2D(GPUTexturePixelType type) {
    return new GLTexture2D(this, type);
}

GPUTexture3D* GLContext::createTexture3D(GPUTexturePixelType type) {
    return new GLTexture3D(this, type);
}

GPUTextureCubemap* GLContext::createTextureCubemap(GPUTexturePixelType type) {
    return new GLTextureCubemap(this, type);
}

GPUTextureCubemapArray* GLContext::createTextureCubemapArray(GPUTexturePixelType type) {
    return new GLTextureCubemapArray(this, type);
}

// ---------------------------------------
// GPU Object Creation: Texture Compressed
// ---------------------------------------

GPUCompressed1D* GLContext::createCompressed1D(GPUTextureCompressedType type) {
    return new GLCompressed1D(this, type);
};

GPUCompressed2D* GLContext::createCompressed2D(GPUTextureCompressedType type) {
    return new GLCompressed2D(this, type);
};

GPUCompressed3D* GLContext::createCompressed3D(GPUTextureCompressedType type) {
    return new GLCompressed3D(this, type);
};

GPUCompressedCubemap* GLContext::createCompressedCubemap(GPUTextureCompressedType type) {
    return new GLCompressedCubemap(this, type);
};

GPUCompressedCubemapArray* GLContext::createCompressedCubemapArray(GPUTextureCompressedType type) {
    return new GLCompressedCubemapArray(this, type);
};

// -----------------------------------
// GPU Object Creation: Render Objects
// -----------------------------------

GPUFrameBuffer* GLContext::createFrameBuffer() {
    return new GLFrameBuffer(this);
}

GPUProgram* GLContext::createProgram() {
    return nullptr;
}

GPUShader* GLContext::createShader(GPUShaderType type, char* buffer, int size) {
    return nullptr;
}

GPUPipeline* GLContext::createPipeline(GPUProgram* program) {
    return nullptr;
}


// -------------------
// GPU Object Commands
// -------------------

void GLContext::gl__makeCurrent() {
    m_driver->makeCurrent(this);
}

GPUCommands* GLContext::createCommands() {
    return nullptr;
}

void GLContext::submit(GPUCommands* commands) {

}

void GLContext::recreateSurface(int w, int h) {

}

void GLContext::swapSurface() {
    gl__makeCurrent();
    glClearColor(0.2, 0.2, 0.3, 0.5);
    glClear(GL_COLOR_BUFFER_BIT);
    // Swap Context Buffer
    LinuxEGLContext *gtx = &m_gtx;
    eglSwapBuffers(gtx->display, gtx->surface);
}

// ---------------------
// GPU Object Destructor
// ---------------------

void GLContext::destroy() {

}
