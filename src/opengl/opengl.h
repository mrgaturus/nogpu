// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_OPENGL_H
#define NOGPU_OPENGL_H
#include <nogpu.h>

class GLContext;
class GLDriver final : public GPUDriver {
    bool impl__checkFeature(GPUDriverFeature feature) override;
    bool impl__shutdown() override;

    // Context Creation: SDL2 & SDL3
    #if defined(NOGPU_SDL2) || defined(NOGPU_SDL3)
        GPUContext *impl__createContext(SDL_Window *win) override;
    #endif

    // GL Driver Initialize
    protected:
        friend GPUDriver;
        GLDriver();
        ~GLDriver();
};

// -----------------
// OpenGL GPU Buffer
// -----------------

class GLBuffer final : public GPUBuffer {
    // GPU Buffer Usage
    void setTarget(GPUBufferTarget m_target) override;
    void orphan(int bytes, GPUBufferUsage usage) override;
    void copy(int bytes, int offset_read, int offset_write, GPUBuffer *data) override;
    void upload(int bytes, void *data, GPUBufferUsage usage) override;
    void update(int bytes, int offset, void *data) override;
    void download(int bytes, int offset, void *data) override;

    // GPU Buffer Usage: Mapping
    void *map(int bytes, int offset, GPUBufferMapping flags) override;
    void unmap() override;

    private: // Buffer Constructor
        friend GLContext;
        void destroy() override;
        GLBuffer();
};

class GLVertexArray final : public GPUVertexArray {
    // GPU Vertex Array: Register
    void attributeArray(GPUBuffer* buffer) override;
    void attributeElements(GPUBuffer* buffer) override;
    void attributeFloat(int index, GPUAttributeSize size, GPUAttributeType type, bool normalized, int stride, int offset) override;
    void attributeInteger(int index, GPUAttributeSize size, GPUAttributeType type, int stride, int offset) override;
    void disableAttribute(int index) override;
    void enableAttribute(int index) override;

    private: // Vertex Array Constructor
        friend GLContext;
        void destroy() override;
        GLVertexArray();
};

// ------------------
// OpenGL GPU Texture
// ------------------

class GLTexture : public GPUTexture {
    // GPU Texture Attributes
    void setSwizzle(GPUTextureFilter swizzle) override;
    void setFilter(GPUTextureFilter filter) override;
    void setWrap(GPUTextureFilter wrap) override;
    void generateMipmaps() override;

    protected: // Texture Constructor
        friend GLContext;
        void destroy() override;
        GLTexture();
};

class GLTexture2D final : public GLTexture, public GPUTexture2D {
    // Texture Buffer Manipulation
    void allocate(int w, int h, int levels) override;
    void upload(int x, int y, int w, int h, int level, void* data) override;
    void download(int x, int y, int w, int h, int level, void* data) override;
    void unpack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int pbo_offset) override;
    void pack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int pbo_offset) override;

    protected: GLTexture2D(
        GPUTexturePixelType m_pixel_type,
        GPUTexturePixelFormat m_pixel_format);
        friend GPUContext;
};

class GLTexture3D final : public GLTexture, public GPUTexture3D {
    // Texture Buffer Manipulation
    void allocate(int w, int h, int layers, int levels) override;
    void upload(int x, int y, int w, int h, int layer, int level, void* data) override;
    void download(int x, int y, int w, int h, int layer, int level, void* data) override;
    void unpack(int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int pbo_offset) override;
    void pack(int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int pbo_offset) override;

    protected: GLTexture3D(
        GPUTexturePixelType m_pixel_type,
        GPUTexturePixelFormat m_pixel_format);
        friend GLContext;
};

class GLTextureCubemap final : public GLTexture, public GPUTextureCubemap {
    // Texture Buffer Manipulation
    void allocate(int w, int h, int levels) override;
    void upload(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, void* data) override;
    void download(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, void* data) override;
    void unpack(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, GPUBuffer *pbo, int pbo_offset) override;
    void pack(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, GPUBuffer *pbo, int pbo_offset) override;

    protected: GLTextureCubemap(
        GPUTexturePixelType m_pixel_type,
        GPUTexturePixelFormat m_pixel_format);
        friend GLContext;
};

// ----------------------
// OpenGL GPU Framebuffer
// ----------------------

class GLRenderbuffer final : public GPURenderbuffer {
    protected: // Renderbuffer Constructor
        GLRenderbuffer(int w, int h, GPUTexturePixelFormat format, int msaa_samples = 0);
        void destroy() override;
        friend GLContext;
};

class GLFramebuffer final : public GPUFramebuffer {
    GPUFramebufferStatus checkStatus() override;
    void attachColor(GPUTexture *color, int index) override;
    void attachDepthStencil(GPUTexture *depth_stencil) override;
    void attachStencil(GPUTexture *stencil) override;
    void attachDepth(GPUTexture *depth) override;
    // GPU Renderbuffer Attachment
    void attachColor(GPURenderbuffer *color, int index) override;
    void attachDepthStencil(GPURenderbuffer *depth_stencil) override;
    void attachStencil(GPURenderbuffer *stencil) override;
    void attachDepth(GPURenderbuffer *depth) override;

    protected: // Framebuffer Constructor
        GLFramebuffer();
        void destroy() override;
        friend GLContext;
};

// -----------------
// OpenGL GPU Shader
// -----------------

class GLProgram;
class GLShader final : public GPUShader {
    bool checkCompile() override;
    std::string checkReport() override;

    protected: // Shader Constructor
        GLShader(GPUShaderType type, char* buffer, int size);
        void destroy() override;
        friend GLContext;
};

class GLUniform : public GPUUniform {
    protected: // Uniform Constructor
        GLUniform(std::string label);
        void destroy() override;
        friend GLProgram;
};

class GLUniformSampler final : public GLUniform, public GPUUniformSampler {
    void setTexture(GPUTexture *texture) override;
    protected: // Uniform Constructor
        GLUniformSampler(std::string label);
        friend GPUProgram;
};

class GLUniformBlock final : public GLUniform, public GPUUniformBlock {
    void setBuffer(GPUBuffer *m_ubo) override;
    protected: // Uniform Constructor
        GLUniformBlock(std::string label, int index);
        friend GPUProgram;
};

class GLUniformValue final : public GLUniform, public GPUUniformValue {
    void setValue(void *value) override;
    void getValue(void *value) override;
    int getTypeSize() override;

    protected: // Uniform Constructor
        GLUniformValue(GPUUniformValueType type, std::string label);
        friend GPUProgram;
};

// ------------------
// OpenGL GPU Program
// ------------------

class GLProgram final : public GPUProgram {
    // GPU Program Shader Attachment
    void attachVertex(GPUShader *vertex) override;
    void attachFragment(GPUShader *fragment) override;
    void attachCompute(GPUShader *compute) override;
    bool compileProgram() override;
    std::string compileReport() override;

    // GPU Program Uniforms
    GPUUniformValue *uniformValue(std::string label) override;
    GPUUniformSampler *uniformSampler(std::string label) override;
    GPUUniformBlock *uniformBlock(std::string label, int index) override;

    protected: // Program Constructor
        GLProgram();
        void destroy() override;
        friend GLContext;
};

// ------------------
// OpenGL GPU Context
// ------------------

class GLContext final : public GPUContext {

};

#endif // NOGPU_OPENGL_H
