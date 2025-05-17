// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "opengl.h"
#include "glad/glad.h"

// -------------------
// GPU Object Creation
// -------------------

GPUVertexArray* GLContext::createVertexArray() {
    return nullptr;
};

GPUBuffer* GLContext::createBuffer(GPUBufferTarget m_target) {
    return nullptr;
}

GPUTexture2D* GLContext::createTexture2D() {
    return nullptr;
}

GPUTexture3D* GLContext::createTexture3D() {
    return nullptr;
}

GPUTextureCubemap* GLContext::createTextureCubemap() {
    return nullptr;
}

GPURenderbuffer* GLContext::createRenderbuffer(int w, int h, GPUTexturePixelFormat format, int msaa_samples) {
    return nullptr;
}

GPUFramebuffer* GLContext::createFramebuffer() {
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

void GLContext::recreateSurface() {

}

void GLContext::swapSurface() {
    gl__makeCurrent();
    glClearColor(0.2, 0.2, 0.3, 0.5);
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(m_egl->display, m_egl_surface);
}

// ---------------------
// GPU Object Destructor
// ---------------------

void GLContext::destroy() {

}
