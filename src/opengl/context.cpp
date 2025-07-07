// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "nogpu/opengl_buffer.h"
#include "nogpu/opengl_context.h"
#include "nogpu/opengl_texture.h"
#include "nogpu/opengl_compressed.h"
#include "glad/glad.h"

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

GPUTextureBuffer* GLContext::createTextureBuffer(GPUTexturePixelType type) {
    return new GLTextureBuffer(this, type);
}

GPURenderBuffer* GLContext::createRenderBuffer(GPURenderBufferMode mode, GPUTexturePixelFormat format) {
    return nullptr;
}

// -------------------------------------
// GPU Object Creation: Texture Standard
// -------------------------------------

GPUTexture1D* GLContext::createTexture1D(GPUTexturePixelType type, GPUTexturePixelFormat format) {
    return new GLTexture1D(this, type, format);
}

GPUTexture2D* GLContext::createTexture2D(GPUTexturePixelType type, GPUTexturePixelFormat format) {
    return new GLTexture2D(this, type, format);
}

GPUTexture3D* GLContext::createTexture3D(GPUTexturePixelType type, GPUTexturePixelFormat format) {
    return new GLTexture3D(this, type, format);
}

GPUTextureCubemap* GLContext::createTextureCubemap(GPUTexturePixelType type, GPUTexturePixelFormat format) {
    return new GLTextureCubemap(this, type, format);
}

GPUTextureCubemapArray* GLContext::createTextureCubemapArray(GPUTexturePixelType type, GPUTexturePixelFormat format) {
    return new GLTextureCubemapArray(this, type, format);
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
    return nullptr;
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
