// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "nogpu_private.h"
#include "private/buffer.h"
#include "private/context.h"
#include "private/texture.h"
#include "private/compressed.h"
#include "private/framebuffer.h"
#include "private/pipeline.h"
#include "private/program.h"
#include "private/glad.h"

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

GPURenderBuffer* GLContext::createRenderBuffer(GPUTexturePixelType type) {
    return new GLRenderBuffer(this, type);
}

GPUShader* GLContext::createShader(GPUShaderType type, GPUShaderSource data) {
    return new GLShader(this, type, data);
}

GPUProgram* GLContext::createProgram() {
    return new GLProgram(this);
}

GPUPipeline* GLContext::createPipeline() {
    return new GLPipeline(this);
}

GPUCommands* GLContext::createCommands() {
    return nullptr;
}

// -------------------------
// GPU Context: Make Current
// -------------------------

void GLContext::makeCurrent(void* object) {
    GPUReport::setObject(object);
    m_driver->makeCurrent(this);
}

void GLContext::makeCurrentTexture(void* object) {
    GPUReport::setObject(object);
    m_driver->makeCurrent(this);
    // Set Active Texture to Stoled Texture Unit
    glActiveTexture(m_device->m_state.stole_texture);
}

// -------------------------
// GPU Context: Global State
// -------------------------

GLState* GLContext::manipulateState() {
    return &m_device->m_state;
}
