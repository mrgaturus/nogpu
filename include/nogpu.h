// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_H
#define NOGPU_H

// Include SDL Window
#if defined(NOGPU_SDL3)
#include <SDL3/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

// -------------------
// GPU Objects: Driver
// -------------------

#if defined(_WIN32)
    enum class GPUDriverOption : int {
        DRIVER_AUTO,
        DRIVER_DX12,
        DRIVER_OPENGL,
        DRIVER_VULKAN
    };
#elif defined(__unix__)
    enum class GPUDriverOption : int {
        DRIVER_AUTO,
        DRIVER_OPENGL,
        DRIVER_VULKAN
    };
#endif

enum class GPUDriverFeature : int {
    DRIVER_FEATURE_RASTERIZE,
    DRIVER_FEATURE_COMPUTE,
    DRIVER_FEATURE_ASYNC,
    // Shader Compiling
    DRIVER_FEATURE_GLSL,
    DRIVER_FEATURE_HLSL,
    DRIVER_FEATURE_SPIRV
};

class GPUContext;
class GPUDriver {
    static GPUDriver *m_driver;
    static GPUDriverOption m_driver_option;
    static unsigned long long m_driver_lock;

    protected: // Driver Virtual Internals
        virtual bool impl__checkFeature(GPUDriverFeature feature) = 0;
        virtual bool impl__shutdown() = 0;
    protected: // Driver Avoid Create
        GPUDriver();
        ~GPUDriver();
    public: // Initialize
        static bool initialize(GPUDriverOption option);
        static bool checkFeature(GPUDriverFeature feature);
        static bool shutdown();

    // Context Creation: SDL2 & SDL3
    #if defined(NOGPU_SDL2) || defined(NOGPU_SDL3)
        protected: virtual GPUContext *impl__createContext(SDL_Window *win) = 0;
        public: static GPUContext *createContext(SDL_Window *win);
    #endif
};

// -------------------
// GPU Objects: Buffer
// -------------------

enum class GPUBufferTarget : int {
    BUFFER_ARRAY,
    BUFFER_COPY_READ,
    BUFFER_COPY_WRITE,
    BUFFER_ELEMENT_ARRAY,
    BUFFER_PIXEL_PACK,
    BUFFER_PIXEL_UNPACK,
    BUFFER_TEXTURE,
    BUFFER_TRANSFORM_FEEDBACK,
    BUFFER_UNIFORM_BUFFER
};

enum class GPUBufferUsage : int {
    BUFFER_USAGE_STREAM_DRAW,
    BUFFER_USAGE_STREAM_READ,
    BUFFER_USAGE_STREAM_COPY,
    // Static Buffer Usage
    BUFFER_USAGE_STATIC_DRAW,
    BUFFER_USAGE_STATIC_READ,
    BUFFER_USAGE_STATIC_COPY,
    // Dynamic Buffer Usage
    BUFFER_USAGE_DYNAMIC_DRAW,
    BUFFER_USAGE_DYNAMIC_READ,
    BUFFER_USAGE_DYNAMIC_COPY,
};

enum class GPUBufferMapping : int {
    BUFFER_MAP_READ_BIT,
    BUFFER_MAP_WRITE_BIT,
    BUFFER_MAP_INVALIDATE_RANGE_BIT,
    BUFFER_MAP_INVALIDATE_BUFFER_BIT,
    BUFFER_MAP_FLUSH_EXPLICIT_BIT,
    BUFFER_MAP_UNSYNCRONIZED_BIT,
};

class GPUVertexArray;
class GPUBuffer {
    GPUBufferTarget m_target;
    void* m_mapping;
    int m_bytes;

    protected: GPUBuffer(GPUBufferTarget m_target);
    public: virtual void destroy() = 0;

    public: // GPU Buffer Usage
        virtual void setTarget(GPUBufferTarget m_target) = 0;
        virtual void orphan(int bytes, GPUBufferUsage usage) = 0;
        virtual void copy(int bytes, int offset_read, int offset_write, GPUBuffer *data) = 0;
        virtual void upload(int bytes, void *data, GPUBufferUsage usage) = 0;
        virtual void update(int bytes, int offset, void *data) = 0;
        virtual void download(int bytes, int offset, void *data) = 0;

    public: // GPU Buffer Usage: Mapping
        virtual void *map(int bytes, int offset, GPUBufferMapping flags) = 0;
        virtual void unmap() = 0;
        virtual void wait() = 0;

    public: // GPU Buffer Attributes
        GPUBufferTarget getTarget() { return m_target; };
        int getBytes() { return m_bytes; };
};

// -------------------------
// GPU Objects: Vertex Array
// -------------------------

enum class GPUAttributeSize : int {
    ATTRIBUTE_SIZE_1,
    ATTRIBUTE_SIZE_2,
    ATTRIBUTE_SIZE_3,
    ATTRIBUTE_SIZE_4
};

enum class GPUAttributeType : int {
    ATTRIBUTE_TYPE_BYTE,
    ATTRIBUTE_TYPE_UNSIGNED_BYTE,
    ATTRIBUTE_TYPE_SHORT,
    ATTRIBUTE_TYPE_UNSIGNED_SHORT,
    ATTRIBUTE_TYPE_INT,
    ATTRIBUTE_TYPE_UNSIGNED_INT,
    ATTRIBUTE_TYPE_FLOAT,
    ATTRIBUTE_TYPE_DOUBLE,
};

class GPUVertexArray {
    protected:
        GPUBuffer* bufferArray;
        GPUBuffer* bufferElements;
    protected: GPUVertexArray();
    public: virtual void destroy() = 0;

    public: // GPU Vertex Array: Register
        virtual void attributeArray(GPUBuffer* buffer) = 0;
        virtual void attributeElements(GPUBuffer* buffer) = 0;
        virtual void attributeFloat(int index, GPUAttributeSize size, GPUAttributeType type, bool normalized, int stride, int offset) = 0;
        virtual void attributeInteger(int index, GPUAttributeSize size, GPUAttributeType type, int stride, int offset) = 0;
        virtual void disableAttribute(int index) = 0;
        virtual void enableAttribute(int index) = 0;
};

// --------------------------
// GPU Enums: Texture Formats
// --------------------------

enum class GPUTextureUploadType : int {
    TEXTURE_UPLOAD_UNSIGNED_BYTE,
    TEXTURE_UPLOAD_BYTE,
    TEXTURE_UPLOAD_UNSIGNED_SHORT,
    TEXTURE_UPLOAD_SHORT,
    TEXTURE_UPLOAD_UNSIGNED_INT,
    TEXTURE_UPLOAD_INT,
    TEXTURE_UPLOAD_FLOAT,
};

enum class GPUTexturePixelType: int {
    TEXTURE_PIXEL_R8,
    TEXTURE_PIXEL_R8_SNORM,
    TEXTURE_PIXEL_R16,
    TEXTURE_PIXEL_R16_SNORM,
    TEXTURE_PIXEL_RG8,
    TEXTURE_PIXEL_RG8_SNORM,
    TEXTURE_PIXEL_RG16,
    TEXTURE_PIXEL_RG16_SNORM,
    TEXTURE_PIXEL_RGB8,
    TEXTURE_PIXEL_RGB8_SNORM,
    TEXTURE_PIXEL_RGB16,
    TEXTURE_PIXEL_RGB16_SNORM,
    TEXTURE_PIXEL_RGBA8,
    TEXTURE_PIXEL_RGBA8_SNORM,
    TEXTURE_PIXEL_RGBA16,
    TEXTURE_PIXEL_RGBA16_SNORM,
    // Depth/stencil formats
    TEXTURE_PIXEL_DEPTH_COMPONENT16,
    TEXTURE_PIXEL_DEPTH_COMPONENT24,
    TEXTURE_PIXEL_DEPTH_COMPONENT32,
    TEXTURE_PIXEL_DEPTH24_STENCIL8,
    TEXTURE_PIXEL_DEPTH32F_STENCIL8,
    // Floating-point formats
    TEXTURE_PIXEL_R16F,
    TEXTURE_PIXEL_RG16F,
    TEXTURE_PIXEL_RGB16F,
    TEXTURE_PIXEL_RGBA16F,
    TEXTURE_PIXEL_R32F,
    TEXTURE_PIXEL_RG32F,
    TEXTURE_PIXEL_RGB32F,
    TEXTURE_PIXEL_RGBA32F,
    // Integer formats
    TEXTURE_PIXEL_R8I,
    TEXTURE_PIXEL_R8UI,
    TEXTURE_PIXEL_R16I,
    TEXTURE_PIXEL_R16UI,
    TEXTURE_PIXEL_R32I,
    TEXTURE_PIXEL_R32UI,
    TEXTURE_PIXEL_RG8I,
    TEXTURE_PIXEL_RG8UI,
    TEXTURE_PIXEL_RG16I,
    TEXTURE_PIXEL_RG16UI,
    TEXTURE_PIXEL_RG32I,
    TEXTURE_PIXEL_RG32UI,
    TEXTURE_PIXEL_RGB8I,
    TEXTURE_PIXEL_RGB8UI,
    TEXTURE_PIXEL_RGB16I,
    TEXTURE_PIXEL_RGB16UI,
    TEXTURE_PIXEL_RGB32I,
    TEXTURE_PIXEL_RGB32UI,
    TEXTURE_PIXEL_RGBA8I,
    TEXTURE_PIXEL_RGBA8UI,
    TEXTURE_PIXEL_RGBA16I,
    TEXTURE_PIXEL_RGBA16UI,
    TEXTURE_PIXEL_RGBA32I,
    TEXTURE_PIXEL_RGBA32UI,

    // Simple Compression
    TEXTURE_PIXEL_COMPRESSED_RED,
    TEXTURE_PIXEL_COMPRESSED_RG,
    TEXTURE_PIXEL_COMPRESSED_RGB,
    TEXTURE_PIXEL_COMPRESSED_RGBA,
    TEXTURE_PIXEL_COMPRESSED_SRGB,
    TEXTURE_PIXEL_COMPRESSED_SRGB_ALPHA,
    // S3TC/DXT compression
    TEXTURE_PIXEL_COMPRESSED_RGB_S3TC_DXT1_EXT,
    TEXTURE_PIXEL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
    TEXTURE_PIXEL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
    TEXTURE_PIXEL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
    // RGTC: Red-Green Texture Compression
    TEXTURE_PIXEL_COMPRESSED_RED_RGTC1,
    TEXTURE_PIXEL_COMPRESSED_SIGNED_RED_RGTC1,
    TEXTURE_PIXEL_COMPRESSED_RG_RGTC2,
    TEXTURE_PIXEL_COMPRESSED_SIGNED_RG_RGTC2,
    // BPTC: Float/UNORM compression
    TEXTURE_PIXEL_COMPRESSED_RGBA_BPTC_UNORM,
    TEXTURE_PIXEL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
    TEXTURE_PIXEL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,
    TEXTURE_PIXEL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,
};

enum class GPUTexturePixelFormat : int {
    TEXTURE_FORMAT_RED,
    TEXTURE_FORMAT_RG,
    TEXTURE_FORMAT_RGB,
    TEXTURE_FORMAT_BGR,
    TEXTURE_FORMAT_RGBA,
    TEXTURE_FORMAT_BGRA,
    TEXTURE_FORMAT_DEPTH_COMPONENT,
    TEXTURE_FORMAT_DEPTH_STENCIL
};

// ------------------------
// GPU Enums: Texture Modes
// ------------------------

enum class GPUTextureFilterMode : int {
    TEXTURE_FILTER_NEAREST,
    TEXTURE_FILTER_LINEAR,
    TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST,
    TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST,
    TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR,
    TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR,
};

enum class GPUTextureSwizzleMode : int {
    TEXTURE_SWIZZLE_R,
    TEXTURE_SWIZZLE_G,
    TEXTURE_SWIZZLE_B,
    TEXTURE_SWIZZLE_A,
    TEXTURE_SWIZZLE_ZERO,
    TEXTURE_SWIZZLE_ONE
};

enum class GPUTextureWrapMode : int {
    TEXTURE_WRAP_CLAMP_TO_EDGE,
    TEXTURE_WRAP_MIRRORED_REPEAT,
    TEXTURE_WRAP_REPEAT,
};

// -------------------------
// GPU Objects: Texture Base
// -------------------------

typedef struct {
    GPUTextureSwizzleMode r;
    GPUTextureSwizzleMode g;
    GPUTextureSwizzleMode b;
    GPUTextureSwizzleMode a;
} GPUTextureSwizzle;

typedef struct {
    GPUTextureFilterMode minify;
    GPUTextureFilterMode magnify;
} GPUTextureFilter;

typedef struct {
    GPUTextureWrapMode s;
    GPUTextureWrapMode t;
    GPUTextureWrapMode r;
} GPUTextureWrap;

typedef struct {
    int w, h;
} GPUTextureSize;

class GPUTexture {
    protected:
        GPUTexturePixelType m_pixel_type;
        GPUTexturePixelFormat m_pixel_format;
        GPUTextureSwizzle m_swizzle;
        GPUTextureFilter m_filter;
        GPUTextureWrap m_wrap;
        int m_w, m_h;
    // Texture Constructor
    protected: GPUTexture();
    public: virtual void destroy() = 0;

    public: // GPU Texture Attributes
        virtual void setSwizzle(GPUTextureFilter swizzle) = 0;
        virtual void setFilter(GPUTextureFilter filter) = 0;
        virtual void setWrap(GPUTextureFilter wrap) = 0;
        virtual void generateMipmaps() = 0;
        virtual void wait() = 0;

    public: // GPU Texture Attributes
        int getW() { return m_w; }
        int getH() { return m_h; }
        GPUTextureSize getSize() { return (GPUTextureSize) { m_w, m_h }; }
        GPUTexturePixelType getPixelType() { return m_pixel_type; }
        GPUTexturePixelFormat getPixelFormat() { return m_pixel_format; }
        GPUTextureSwizzle getPixelSwizzle() { return m_swizzle; }
        GPUTextureFilter getPixelFilter() { return m_filter; }
        GPUTextureWrap getPixelWrap() { return m_wrap; }
};

// --------------------
// GPU Objects: Texture
// --------------------

class GPUTexture2D : GPUTexture {
    protected: GPUTexture2D(
        GPUTexturePixelType m_pixel_type,
        GPUTexturePixelFormat m_pixel_format);

    public: // Texture Buffer Manipulation
        virtual void allocate(int w, int h, int levels) = 0;
        virtual void upload(int x, int y, int w, int h, int level, void* data) = 0;
        virtual void download(int x, int y, int w, int h, int level, void* data) = 0;
        virtual void unpack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int pbo_offset) = 0;
        virtual void pack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int pbo_offset) = 0;
};

enum class GPUTexture3DMode : int {
    TEXTURE_2D_ARRAY,
    TEXTURE_3D,
};

class GPUTexture3D : GPUTexture {
    protected:
        GPUTexture3DMode m_mode;
        int m_layers;
    protected: GPUTexture3D(
        GPUTexturePixelType m_pixel_type,
        GPUTexturePixelFormat m_pixel_format);

    public: // Texture Buffer Manipulation
        virtual void allocate(int w, int h, int layers, int levels) = 0;
        virtual void upload(int x, int y, int w, int h, int layer, int level, void* data) = 0;
        virtual void download(int x, int y, int w, int h, int layer, int level, void* data) = 0;
        virtual void unpack(int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int pbo_offset) = 0;
        virtual void pack(int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int pbo_offset) = 0;
    public: // Attribute Getters
        GPUTexture3DMode getMode() { return m_mode; };
        int getLayers() { return m_layers; };
};

enum class GPUTextureCubemapSide : int {
    TEXTURE_CUBE_MAP_POSITIVE_X,
    TEXTURE_CUBE_MAP_POSITIVE_Y,
    TEXTURE_CUBE_MAP_POSITIVE_Z,
    TEXTURE_CUBE_MAP_NEGATIVE_X,
    TEXTURE_CUBE_MAP_NEGATIVE_Y,
    TEXTURE_CUBE_MAP_NEGATIVE_Z
};

class GPUTextureCubemap : GPUTexture {
    protected: GPUTextureCubemap(
        GPUTexturePixelType m_pixel_type,
        GPUTexturePixelFormat m_pixel_format);
    public: // Texture Buffer Manipulation
        virtual void allocate(int w, int h, int levels) = 0;
        virtual void upload(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, void* data) = 0;
        virtual void download(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, void* data) = 0;
        virtual void unpack(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, GPUBuffer *pbo, int pbo_offset) = 0;
        virtual void pack(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, GPUBuffer *pbo, int pbo_offset) = 0;
};

// ------------------------
// GPU Objects: Framebuffer
// ------------------------

class GPURenderbuffer {
    protected:
        GPUTexturePixelFormat m_format;
        int m_w, m_h;
    // GPU Renderbuffer Constructor
    protected: GPURenderbuffer(int w, int h, GPUTexturePixelFormat format, int msaa_samples = 0);
    public: virtual void destroy() = 0;

    public: // GPU Renderbuffer Attributes
        int getW() { return m_w; }
        int getH() { return m_h; }
        GPUTextureSize getSize() { return (GPUTextureSize) { m_w, m_h }; }
        GPUTexturePixelFormat getFormat() { return m_format; }
};

enum class GPUFramebufferStatus : int {
    FRAMEBUFFER_UNDEFINED,
    FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
    FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
    FRAMEBUFFER_UNSUPPORTED,
    FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
};

class GPUFramebuffer {
    protected:
        GPUTexture *m_colors[32];
        GPUTexture *m_stencil;
        GPUTexture *m_depth;
    // GPU Framebuffer Constructor
    protected: GPUFramebuffer();
    public: virtual void destroy() = 0;

    public: // GPU Texture Attach
        virtual GPUFramebufferStatus checkStatus() = 0;
        virtual void attachColor(GPUTexture *color, int index) = 0;
        virtual void attachDepthStencil(GPUTexture *depth_stencil) = 0;
        virtual void attachStencil(GPUTexture *stencil) = 0;
        virtual void attachDepth(GPUTexture *depth) = 0;
        // GPU Renderbuffer Attachment
        virtual void attachColor(GPURenderbuffer *color, int index) = 0;
        virtual void attachDepthStencil(GPURenderbuffer *depth_stencil) = 0;
        virtual void attachStencil(GPURenderbuffer *stencil) = 0;
        virtual void attachDepth(GPURenderbuffer *depth) = 0;
    public: // GPU Framebuffer Attributes
        GPUTexture *getColor(int index) { return m_colors[index]; }
        GPUTexture *getStencil() { return m_stencil; }
        GPUTexture *getDepth() { return m_depth; }
};

// ---------------------------
// GPU Objects: Program Shader
// ---------------------------

enum class GPUShaderType : int {
    SHADER_VERTEX,
    SHADER_FRAGMENT,
    SHADER_COMPUTE
};

class GPUProgram;
class GPUShader {
    protected: GPUShaderType m_type;
    protected: GPUShader(GPUShaderType type, char* buffer, int size);
    public: virtual void destroy() = 0;

    public: // GPU Shader Attributes
        virtual bool checkCompile() = 0;
        virtual char* checkReport() = 0;
        GPUShaderType getType() { return m_type; };
};

// ----------------------------
// GPU Objects: Program Uniform
// ----------------------------

class GPUUniform {
    protected:
        char* m_label;
        GPUProgram *program;
        GPUUniform(char* label);
    public: virtual void destroy() = 0;
    public: char* getLabel() { return m_label; };
};

class GPUUniformSampler : GPUUniform {
    protected: GPUTexture *m_texture;
    protected: GPUUniformSampler(char* label);
    public: // GPU Sampler Attributes
        virtual void setTexture(GPUTexture *texture) = 0;
        GPUTexture *getTexture() { return m_texture; };
};

enum class GPUUniformBlockType : int {
    BLOCK_UNIFORM_BUFFER,
    BLOCK_SHADER_STORAGE_BUFFER,
    BLOCK_ATOMIC_COUNTER_BUFFER
};

class GPUUniformBlock : GPUUniform {
    protected: // GPU Uniform Block
        GPUUniformBlockType m_type;
        GPUBuffer *m_ubo;
        int m_index;

    protected: GPUUniformBlock(char* label, int index);
    public: // GPU Sampler Attributes
        virtual void setBuffer(GPUBuffer *m_ubo) = 0;
        GPUBuffer *getBuffer() { return m_ubo; };
        int getIndex() { return m_index; };
};

enum class GPUUniformValueType : int {
    UNIFORM_INT,
    UNIFORM_INTx2,
    UNIFORM_INTx3,
    UNIFORM_INTx4,
    UNIFORM_UNSIGNED_INT,
    UNIFORM_UNSIGNED_INT_x2,
    UNIFORM_UNSIGNED_INT_x3,
    UNIFORM_UNSIGNED_INT_x4,
    UNIFORM_FLOAT,
    UNIFORM_FLOAT_x2,
    UNIFORM_FLOAT_x3,
    UNIFORM_FLOAT_x4,
    // Uniform Matrix
    UNIFORM_MATRIX_2x2,
    UNIFORM_MATRIX_3x3,
    UNIFORM_MATRIX_4x4,
    UNIFORM_MATRIX_2x3,
    UNIFORM_MATRIX_3x2,
    UNIFORM_MATRIX_2x4,
    UNIFORM_MATRIX_4x2,
    UNIFORM_MATRIX_3x4,
    UNIFORM_MATRIX_4x3,
    // Transposed Uniform Matrix
    UNIFORM_MATRIX_TRANSPOSED_2x2,
    UNIFORM_MATRIX_TRANSPOSED_3x3,
    UNIFORM_MATRIX_TRANSPOSED_4x4,
    UNIFORM_MATRIX_TRANSPOSED_2x3,
    UNIFORM_MATRIX_TRANSPOSED_3x2,
    UNIFORM_MATRIX_TRANSPOSED_2x4,
    UNIFORM_MATRIX_TRANSPOSED_4x2,
    UNIFORM_MATRIX_TRANSPOSED_3x4,
    UNIFORM_MATRIX_TRANSPOSED_4x3,
};

class GPUUniformValue : GPUUniform {
    protected: // GPU Uniform Value
        GPUUniformValueType m_type;
        unsigned int m_value[16];

    protected: GPUUniformValue(GPUUniformValueType type, char* label);
    public: // GPU Sampler Attributes
        virtual void setValue(void *value) = 0;
        virtual void getValue(void *value) = 0;
        GPUUniformValueType getType() { return m_type; };
        virtual int getTypeSize() = 0;
};

// --------------------
// GPU Objects: Program
// --------------------

class GPUProgram {
    protected: GPUProgram();
    public: virtual void destroy() = 0;

    public: // GPU Program Shader Attachment
        virtual void attachVertex(GPUShader *vertex) = 0;
        virtual void attachFragment(GPUShader *fragment) = 0;
        virtual void attachCompute(GPUShader *compute) = 0;
        virtual bool compileProgram() = 0;
        virtual char* compileReport() = 0;

    public: // GPU Program Uniforms
        virtual GPUUniformValue *uniformValue(char* label) = 0;
        virtual GPUUniformSampler *uniformSampler(char* label) = 0;
        virtual GPUUniformBlock *uniformBlock(char* label, int index) = 0;
        virtual GPUUniform *getUniform(char* label) = 0;
};

// -----------------------------
// GPU Context: State Primitives
// -----------------------------

typedef struct {
    bool x, y, w, h;
} GPURectangle;

typedef struct {
    float r, g, b, a;
} GPUColor;

typedef struct {
    bool r, g, b, a;
} GPUColorMask;

// ---------------------------
// GPU Context: Pipeline State
// ---------------------------

enum class GPUBlendEquation : int {
    BLEND_FUNC_ADD,
    BLEND_FUNC_SUBSTRACT,
    BLEND_FUNC_REVERSE_SUBSTRACT,
    BLEND_FUNC_MIN,
    BLEND_FUNC_MAX
};

enum class GPUBlendFactor : int {
    BLEND_ZERO,
    BLEND_ONE,
    BLEND_SRC_COLOR,
    BLEND_ONE_MINUS_SRC_COLOR,
    BLEND_DST_COLOR,
    BLEND_ONE_MINUS_DST_COLOR,
    BLEND_SRC_ALPHA,
    BLEND_ONE_MINUS_SRC_ALPHA,
    BLEND_DST_ALPHA,
    BLEND_ONE_MINUS_DST_ALPHA,
    BLEND_CONSTANT_COLOR,
    BLEND_ONE_MINUS_CONSTANT_COLOR,
    BLEND_CONSTANT_ALPHA,
    BLEND_ONE_MINUS_CONSTANT_ALPHA,
    BLEND_SRC_ALPHA_SATURATE,
};

enum class GPUConditionFunc : int {
    CONDITION_NEVER,
    CONDITION_EQUAL,
    CONDITION_NOTEQUAL,
    CONDITION_LESS,
    CONDITION_LEQUAL,
    CONDITION_GREATER,
    CONDITION_GEQUAL,
    CONDITION_ALWAYS,
};

enum class GPUStencilFunc : int {
    STENCIL_KEEP,
    STENCIL_ZERO,
    STENCIL_REPLACE,
    STENCIL_INCR,
    STENCIL_INCR_WRAP,
    STENCIL_DECR,
    STENCIL_DECR_WRAP,
    STENCIL_INVERT
};

enum class GPUFaceMode : int {
    FACE_BACK,
    FACE_FRONT,
    FACE_FRONT_AND_BACK
};

enum class GPUWindingMode : int {
    WINDING_CW,
    WINDING_CCW,
};

// ---------------------
// GPU Context: Pipeline
// ---------------------

enum class GPUPipelineCapability : int {
    CAPABILITY_BLENDING,
    CAPABILITY_FACE_CULL,
    CAPABILITY_DEPTH_TEST,
    CAPABILITY_DEPTH_BIAS,
    CAPABILITY_PRIMITIVE_RESTART,
    CAPABILITY_RASTERIZER_DISCARD,
    CAPABILITY_SCISSOR_TEST,
    CAPABILITY_STENCIL_TEST
};

typedef struct {
    struct GPUPipelineBlendingEquation {
        GPUBlendEquation rgb;
        GPUBlendEquation alpha;
    } equation;

    struct GPUPipelineBlendingFactor {
        GPUBlendFactor srcRGB, srcAlpha;
        GPUBlendFactor dstRGB, dstAlpha;
    } factor;

    GPUColor constantColor;
} GPUPipelineBlending;

typedef struct {
    GPUFaceMode cull;
    GPUWindingMode front;
} GPUPipelineFace;

typedef struct {
    GPUConditionFunc func;
    bool mask;

    struct GPUPipelineDepthBias {
        float constantFactor;
        float slopeFactor;
        float clamp;
    } bias;
} GPUPipelineDepth;

typedef struct {
    struct GPUPipelineStencilFunc {
        GPUFaceMode face;
        GPUConditionFunc func;
        int test; unsigned int mask;
    } func;

    struct GPUPipelineStencilMask {
        GPUFaceMode face;
        unsigned int mask;
    } mask;

    struct GPUPipelineStencilOp {
        GPUFaceMode face;
        GPUStencilFunc fail;
        GPUStencilFunc pass;
        GPUStencilFunc depth_pass;
    } op;
} GPUPipelineStencil;

class GPUPipeline {
    GPUProgram* m_program;

    unsigned int m_capabilities;
    GPUPipelineBlending m_blending;
    GPUPipelineFace m_face;
    GPUPipelineDepth m_depth;
    GPUPipelineStencil m_stencil;

    float m_lineWidth;
    GPUColor m_clearColor;
    GPUColorMask m_maskColor;
    GPURectangle m_scissor;
    GPURectangle m_viewport;

    // Pipeline Constructor
    protected: GPUPipeline(GPUProgram* program);
    public: virtual void destroy() { delete this; };

    public: // Pipeline Capabilites
        bool check(GPUPipelineCapability cap) { return (m_capabilities & (1 << (unsigned int) cap)) != 0; }
        virtual void enable(GPUPipelineCapability cap) { m_capabilities |= (1 << (unsigned int) cap); }
        virtual void disable(GPUPipelineCapability cap) { m_capabilities &= ~(1 << (unsigned int) cap); }

    public: // Pipeline Attributes: Setters
        virtual void setProgram(GPUProgram* program) { m_program = program; };
        virtual void setBlending(GPUPipelineBlending blending) { m_blending = blending; }
        virtual void setFace(GPUPipelineFace face) { m_face = face; }
        virtual void setDepth(GPUPipelineDepth depth) { m_depth = depth; }
        virtual void setStencil(GPUPipelineStencil stencil) { m_stencil = stencil; }
        // GPU Context State: Viewport
        virtual void setLineWidth(float width) { m_lineWidth = width; }
        virtual void setClearColor(GPUColor color) { m_clearColor = color; }
        virtual void setMaskColor(GPUColorMask mask) { m_maskColor = mask; }
        virtual void setViewport(GPURectangle rect) { m_viewport = rect; }
        virtual void setScissor(GPURectangle rect) { m_scissor = rect; }

    public: // Pipeline Attributes: Getters
        GPUPipelineBlending getBlending() { return m_blending; }
        GPUPipelineFace getFace() { return m_face; }
        GPUPipelineDepth getDepth() { return m_depth; }
        GPUPipelineStencil getStencil() { return m_stencil; }
        // GPU Context State: Viewport
        float getLineWidth() { return m_lineWidth; }
        GPUColor getClearColor() { return m_clearColor; }
        GPUColorMask getMaskColor() { return m_maskColor; }
        GPURectangle getViewport() { return m_viewport; }
        GPURectangle getScissor() { return m_scissor; }
};

// ---------------------
// GPU Context: Commands
// ---------------------

enum class GPUDrawPrimitive : int {
    PRIMITIVE_POINTS,
    PRIMITIVE_LINES,
    PRIMITIVE_LINE_STRIP,
    PRIMITIVE_LINE_LOOP,
    PRIMITIVE_TRIANGLES,
    PRIMITIVE_TRIANGLE_STRIP,
    PRIMITIVE_TRIANGLE_FAN,
};

enum class GPUDrawElementsType : int {
    ELEMENTS_UNSIGNED_BYTE,
    ELEMENTS_UNSIGNED_SHORT,
    ELEMENTS_UNSIGNED_INT
};

typedef enum {
    BARRIER_VEXTEX_ATTRIB_ARRAY = 1 << 0,
    BARRIER_ELEMENT_ARRAY = 1 << 1,
    BARRIER_UNIFORM = 1 << 2,
    BARRIER_TEXTURE_FETCH = 1 << 3,
    BARRIER_SHADER_IMAGE_ACCESS = 1 << 4,
    BARRIER_COMMAND = 1 << 5,
    BARRIER_PIXEL_BUFFER = 1 << 6,
    BARRIER_TEXTURE_UPDATE = 1 << 7,
    BARRIER_BUFFER_UPDATE = 1 << 8,
    BARRIER_FRAMEBUFFER = 1 << 9,
    BARRIER_ATOMIC_COUNTER = 1 << 10,
    BARRIER_SHADER_STORAGE = 1 << 11,
    BARRIER_ALL = 0x7FFFFFFF,
} GPUMemoryBarrierFlags;

typedef struct {
    GPUPipeline *pipeline;
    GPUPipeline pipelineCache;
    GPUBuffer *buffers[16];
    GPUTexture *textures[64];
    GPUVertexArray *vertexArray;
    GPURenderbuffer *renderBuffer;
    GPUFramebuffer *framebufferRead;
    GPUFramebuffer *framebufferDraw;
} GPUCommandState;

class GPUCommands {
    protected:
        GPUCommands();
        GPUCommandState m_state;
        bool m_recorded;
    public: virtual void destroy() = 0;

    public: // GPU Command Record 
        bool checkRecord() { return m_recorded; }
        virtual void beginRecord() = 0;
        virtual void endRecord() = 0;
        virtual void wait() = 0;

    public: // GPU Command State
        virtual void usePipeline(GPUPipeline *pipeline) = 0;
        virtual void useVertexArray(GPUVertexArray *vertex_array) = 0;
        virtual void useBuffer(GPUBuffer *buffer, GPUBufferTarget target) = 0;
        virtual void useTexture(GPUTexture *texture, int index) = 0;
        virtual void useFramebuffer(GPUFramebuffer* draw) = 0;
        virtual void useFramebuffer(GPUFramebuffer* draw, GPUFramebuffer* read) = 0;
        virtual void useFramebufferContext() = 0;

    public: // GPU Command Rendering
        virtual void drawClear() = 0;
        virtual void drawArrays(GPUDrawPrimitive type, int offset, int count) = 0;
        virtual void drawElements(GPUDrawPrimitive type, int offset, int count, GPUDrawElementsType element) = 0;
        virtual void drawElementsBaseVertex(GPUDrawPrimitive type, int offset, int count, int base, GPUDrawElementsType element) = 0;
        virtual void drawArraysInstanced(GPUDrawPrimitive type, int offset, int count, int instance_count) = 0;
        virtual void drawElementsInstanced(GPUDrawPrimitive type, int offset, int count, GPUDrawElementsType element, int instance_count) = 0;
        virtual void drawElementsBaseVertexInstanced(GPUDrawPrimitive type, int offset, int count, int base, GPUDrawElementsType element, int instance_count) = 0;
        virtual void executeComputeAsync(unsigned int num_groups_x, unsigned int num_groups_y, unsigned int num_groups_z) = 0;
        virtual void executeCompute(unsigned int num_groups_x, unsigned int num_groups_y, unsigned int num_groups_z) = 0;
        virtual void memoryBarrier(GPUMemoryBarrierFlags barriers) = 0;
};

// -----------
// GPU Context
// -----------

class GPUContext {
    protected:
        GPUContext();
        static GPUContext* m_current;
    public: virtual void destroy() = 0;

    public: // GPU Objects Creation
        virtual GPUVertexArray* createVertexArray() = 0;
        virtual GPUBuffer* createBuffer(GPUBufferTarget m_target) = 0;
        virtual GPUTexture2D* createTexture2D() = 0;
        virtual GPUTexture3D* createTexture3D() = 0;
        virtual GPUTextureCubemap* createTextureCubemap() = 0;
        virtual GPURenderbuffer* createRenderbuffer(int w, int h, GPUTexturePixelFormat format, int msaa_samples = 0) = 0;
        virtual GPUFramebuffer* createFramebuffer() = 0;
        virtual GPUProgram* createProgram() = 0;
        virtual GPUShader* createShader(GPUShaderType type, char* buffer, int size) = 0;
        virtual GPUPipeline* createPipeline(GPUProgram* program) = 0;

    public: // GPU Object Commands
        virtual GPUCommands* createCommands() = 0;
        virtual void submit(GPUCommands* commands) = 0;
        virtual void recreateSurface() = 0;
        virtual void swapSurface() = 0;
};

#endif // NOGPU_H
