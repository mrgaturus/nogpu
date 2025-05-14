// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_OPENGL_H
#define NOGPU_OPENGL_H
#include <nogpu.h>

#if defined(__unix__)
#include <EGL/egl.h>
#endif

class GLContext;
class GLDriver : GPUDriver {
    unsigned int m_features = 0;
    int m_msaa_samples = 0;

    #if defined(__unix__)
        // X11 EGL Context
        EGLDisplay m_egl_display_x11;
        EGLConfig m_egl_config_x11;
        EGLContext m_egl_context_x11;
        // Wayland EGL Context
        EGLDisplay m_egl_display_wayland;
        EGLConfig m_egl_config_wayland;
        EGLContext m_egl_context_wayland;
        // Current EGL Context/Surface
        EGLContext m_egl_context;
        EGLSurface m_egl_surface;
    #endif

    bool impl__checkInitialized() override;
    bool impl__checkFeature(GPUDriverFeature feature) override;
    GPUDriverOption impl__getDriverOption() override;
    int impl__getMultisamplesCount() override;
    bool impl__shutdown() override;

    // Context Creation: SDL2 & SDL3
    #if defined(NOGPU_SDL2) || defined(NOGPU_SDL3)
        GPUContext *impl__createContext(SDL_Window *win) override;
    #endif

    // GL Driver Initialize
    protected:
        friend GPUDriver;
        GLDriver(int msaa_samples = 0);
        ~GLDriver();
};

// -----------------
// OpenGL GPU Buffer
// -----------------

class GLBuffer : GPUBuffer {
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
    void wait() override;

    private: // Buffer Constructor
        friend GLContext;
        void destroy() override;
        GLBuffer();
};

class GLVertexArray : GPUVertexArray {
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

class GLTexture : GPUTexture {
    // GPU Texture Attributes
    void setSwizzle(GPUTextureFilter swizzle) override;
    void setFilter(GPUTextureFilter filter) override;
    void setWrap(GPUTextureFilter wrap) override;
    void generateMipmaps() override;
    void wait() override;

    protected: // Texture Constructor
        friend GLContext;
        void destroy() override;
        GLTexture();
};

class GLTexture2D : GLTexture, GPUTexture2D {
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

class GLTexture3D : GLTexture, GPUTexture3D {
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

class GLTextureCubemap : GLTexture, GPUTextureCubemap {
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

class GLRenderbuffer : GPURenderbuffer {
    protected: // Renderbuffer Constructor
        GLRenderbuffer(int w, int h, GPUTexturePixelFormat format, int msaa_samples = 0);
        void destroy() override;
        friend GLContext;
};

class GLFramebuffer : GPUFramebuffer {
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
class GLShader : GPUShader {
    bool checkCompile() override;
    char* checkReport() override;

    protected: // Shader Constructor
        GLShader(GPUShaderType type, char* buffer, int size);
        void destroy() override;
        friend GLContext;
};

class GLUniform : GPUUniform {
    protected: // Uniform Constructor
        GLUniform(char* label);
        void destroy() override;
        friend GLProgram;
};

class GLUniformSampler : GLUniform, GPUUniformSampler {
    void setTexture(GPUTexture *texture) override;
    protected: // Uniform Constructor
        GLUniformSampler(char* label);
        friend GPUProgram;
};

class GLUniformBlock : GLUniform, GPUUniformBlock {
    void setBuffer(GPUBuffer *m_ubo) override;
    protected: // Uniform Constructor
        GLUniformBlock(char* label, int index);
        friend GPUProgram;
};

class GLUniformValue : GLUniform, GPUUniformValue {
    void setValue(void *value) override;
    void getValue(void *value) override;
    int getTypeSize() override;

    protected: // Uniform Constructor
        GLUniformValue(GPUUniformValueType type, char* label);
        friend GPUProgram;
};

// ------------------
// OpenGL GPU Program
// ------------------

class GLProgram : GPUProgram {
    // GPU Program Shader Attachment
    void attachVertex(GPUShader *vertex) override;
    void attachFragment(GPUShader *fragment) override;
    void attachCompute(GPUShader *compute) override;
    bool compileProgram() override;
    char* compileReport() override;

    // GPU Program Uniforms
    GPUUniformValue *uniformValue(char* label) override;
    GPUUniformSampler *uniformSampler(char* label) override;
    GPUUniformBlock *uniformBlock(char* label, int index) override;
    GPUUniform *removeUniform(char* label) override;
    GPUUniform *getUniform(char* label) override;

    protected: // Program Constructor
        GLProgram();
        void destroy() override;
        friend GLContext;
};

// -------------------
// OpenGL GPU Commands
// -------------------

class GLCommands : GPUCommands {
    // GPU Command Record
    void beginRecord() override;
    void endRecord() override;
    void wait() override;

    // GPU Command State
    void usePipeline(GPUPipeline *pipeline) override;
    void useVertexArray(GPUVertexArray *vertex_array) override;
    void useBuffer(GPUBuffer *buffer, GPUBufferTarget target) override;
    void useTexture(GPUTexture *texture, int index) override;
    void useFramebuffer(GPUFramebuffer* draw) override;
    void useFramebuffer(GPUFramebuffer* draw, GPUFramebuffer* read) override;
    void useFramebufferContext() override;

    // GPU Command Rendering
    void drawClear() override;
    void drawArrays(GPUDrawPrimitive type, int offset, int count) override;
    void drawElements(GPUDrawPrimitive type, int offset, int count, GPUDrawElementsType element) override;
    void drawElementsBaseVertex(GPUDrawPrimitive type, int offset, int count, int base, GPUDrawElementsType element) override;
    void drawArraysInstanced(GPUDrawPrimitive type, int offset, int count, int instance_count) override;
    void drawElementsInstanced(GPUDrawPrimitive type, int offset, int count, GPUDrawElementsType element, int instance_count) override;
    void drawElementsBaseVertexInstanced(GPUDrawPrimitive type, int offset, int count, int base, GPUDrawElementsType element, int instance_count) override;
    void executeComputeSync(unsigned int num_groups_x, unsigned int num_groups_y, unsigned int num_groups_z) override;
    void executeCompute(unsigned int num_groups_x, unsigned int num_groups_y, unsigned int num_groups_z) override;
    void memoryBarrier(GPUMemoryBarrierFlags from, GPUMemoryBarrierFlags to) override;

    protected: // Commands Constructor
        GLCommands();
        void destroy() override;
        friend GLContext;
};

// ------------------
// OpenGL GPU Context
// ------------------

class GLContext : GPUContext {
    void* m_window;
    EGLSurface egl_context;
    EGLSurface egl_surface;
    // GPU Object Creation
    GPUVertexArray* createVertexArray() override;
    GPUBuffer* createBuffer(GPUBufferTarget m_target) override;
    GPUTexture2D* createTexture2D() override;
    GPUTexture3D* createTexture3D() override;
    GPUTextureCubemap* createTextureCubemap() override;
    GPURenderbuffer* createRenderbuffer(int w, int h, GPUTexturePixelFormat format, int msaa_samples = 0) override;
    GPUFramebuffer* createFramebuffer() override;
    GPUProgram* createProgram() override;
    GPUShader* createShader(GPUShaderType type, char* buffer, int size) override;
    GPUPipeline* createPipeline(GPUProgram* program) override;

    // GPU Object Commands
    GPUCommands* createCommands() override;
    void submit(GPUCommands* commands) override;
    void recreateSurface() override;
    void swapSurface() override;

    protected: // Commands Constructor
        GLContext();
        void destroy() override;
        friend GPUDriver;
};

#endif // NOGPU_OPENGL_H
