// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "nogpu/opengl_buffer.h"
#include "nogpu/opengl_context.h"
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
    return nullptr;
};

GPUTextureBuffer* GLContext::createTextureBuffer(GPUTexturePixelFormat format) {
    return nullptr;
}

GPUTexture1D* GLContext::createTexture1D(GPUTexturePixelType type, GPUTexturePixelFormat format) {
    return nullptr;
}

GPUTexture2D* GLContext::createTexture2D(GPUTexturePixelType type, GPUTexturePixelFormat format) {
    return nullptr;
}

GPUTexture3D* GLContext::createTexture3D(GPUTexturePixelType type, GPUTexturePixelFormat format) {
    return nullptr;
}

GPUTextureCubemap* GLContext::createTextureCubemap(GPUTexturePixelType type, GPUTexturePixelFormat format) {
    return nullptr;
}

GPUTextureCubemapArray* GLContext::createTextureCubemapArray(GPUTexturePixelType type, GPUTexturePixelFormat format) {
    return nullptr;
}

GPURenderBuffer* GLContext::createRenderBuffer(GPURenderBufferMode mode, GPUTexturePixelFormat format) {
    return nullptr;
}

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
