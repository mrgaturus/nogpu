#ifndef NOGPU_H
#define NOGPU_H
#include <SDL3/SDL.h>
#include <string>
#include <mutex>

    #ifdef _WIN32
        enum class GPUDriverOption : int {
            DRIVER_AUTO,
            DRIVER_DX11,
            DRIVER_OPENGL,
            DRIVER_VULKAN
        };
    #else
        enum class GPUDriverOption : int {
            DRIVER_AUTO,
            DRIVER_OPENGL,
            DRIVER_VULKAN
        };
    #endif
    
    class GPUContext;
    class GPUDriver {
        friend GPUContext;
        // Global Driver Instance
        static GPUDriver *m_driver;
        static std::mutex m_driver_mutex;
        GPUDriverOption m_driver_option;

        public: // Initialize
            static bool initialize(GPUDriverOption option);
            virtual GPUContext *createContext(SDL_Window* win);
            static bool shutdown();
        protected:
            GPUDriver();
            ~GPUDriver();
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

        friend GPUVertexArray;
        protected: GPUBuffer(GPUBufferTarget m_target);
        public: virtual ~GPUBuffer();

        public: // GPU Buffer Usage
            virtual void setTarget(GPUBufferTarget m_target);
            virtual void orphan(int bytes, GPUBufferUsage usage);
            virtual void copy(int bytes, int offset_read, int offset_write, GPUBuffer *data);
            virtual void upload(int bytes, void *data, GPUBufferUsage usage);
            virtual void update(int bytes, int offset, void *data);
            virtual void download(int bytes, int offset, void *data);

        public: // GPU Buffer Usage: Mapping
            virtual void *map(int bytes, int offset, GPUBufferMapping flags);
            virtual void unmap();

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
        GPUBuffer* bufferArray;
        GPUBuffer* bufferElements;

        protected: GPUVertexArray();
        public: virtual ~GPUVertexArray();

        public: // GPU Vertex Array: Register
            virtual void attributeArray(GPUBuffer* buffer);
            virtual void attributeElements(GPUBuffer* buffer);
            virtual void attributeFloat(int index, GPUAttributeSize size, GPUAttributeType type, bool normalized, int stride, int offset);
            virtual void attributeInteger(int index, GPUAttributeSize size, GPUAttributeType type, int stride, int offset);
            virtual void disableAttribute(int index);
            virtual void enableAttribute(int index);
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
        GPUTexturePixelType m_pixel_type;
        GPUTexturePixelFormat m_pixel_format;
        GPUTextureSwizzle m_swizzle;
        GPUTextureFilter m_filter;
        GPUTextureWrap m_wrap;
        int m_w, m_h;

        private: GPUTexture();
        public: virtual ~GPUTexture();

        public: // GPU Texture Attributes
            virtual void setSwizzle(GPUTextureFilter swizzle);
            virtual void setFilter(GPUTextureFilter filter);
            virtual void setWrap(GPUTextureFilter wrap);
            virtual void generateMipmaps();

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
        protected: GPUTexture2D();
        public: // Texture Buffer Manipulation
            virtual void allocate(int w, int h, int levels);
            virtual void upload(int x, int y, int w, int h, int level, void* data);
            virtual void download(int x, int y, int w, int h, int level, void* data);
            virtual void unpack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int pbo_offset);
            virtual void pack(int x, int y, int w, int h, int level, GPUBuffer *pbo, int pbo_offset);
    };

    enum class GPUTexture3DMode : int {
        TEXTURE_2D_ARRAY,
        TEXTURE_3D,
    };

    class GPUTexture3D : GPUTexture {
        private:
            GPUTexture3DMode m_mode;
            int m_layers;

        protected: GPUTexture3D();
        public: // Texture Buffer Manipulation
            virtual void allocate(int w, int h, int layers, int levels);
            virtual void upload(int x, int y, int w, int h, int layer, int level, void* data);
            virtual void download(int x, int y, int w, int h, int layer, int level, void* data);
            virtual void unpack(int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int pbo_offset);
            virtual void pack(int x, int y, int w, int h, int layer, int level, GPUBuffer *pbo, int pbo_offset);
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
        protected: GPUTextureCubemap();
        public: // Texture Buffer Manipulation
            virtual void allocate(int w, int h, int levels);
            virtual void upload(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, void* data);
            virtual void download(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, void* data);
            virtual void unpack(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, GPUBuffer *pbo, int pbo_offset);
            virtual void pack(int x, int y, int w, int h, int level, GPUTextureCubemapSide side, GPUBuffer *pbo, int pbo_offset);
    };

    // ------------------------
    // GPU Objects: Framebuffer
    // ------------------------

    class GPURenderbuffer {
        GPUTexturePixelFormat m_format;
        int m_w, m_h;

        protected: GPURenderbuffer(int w, int h, GPUTexturePixelFormat format, int msaa_samples = 0);
        public: virtual ~GPURenderbuffer();

        public: // GPU RenderBuffer Attributes
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
        GPUTexture *m_colors[32];
        GPUTexture *m_stencil;
        GPUTexture *m_depth;

        protected: GPUFramebuffer();
        public: virtual ~GPUFramebuffer();

        public:
            virtual GPUFramebufferStatus checkStatus();
            virtual void attachColor(GPUTexture* color, int index);
            virtual void attachDepthStencil(GPUTexture* depth_stencil);
            virtual void attachStencil(GPUTexture* stencil);
            virtual void attachDepth(GPUTexture* depth);
            // GPU Renderbuffer Attachment
            virtual void attachColor(GPURenderbuffer* color, int index);
            virtual void attachDepthStencil(GPURenderbuffer* depth_stencil);
            virtual void attachStencil(GPURenderbuffer* stencil);
            virtual void attachDepth(GPURenderbuffer* depth);
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
        GPUShaderType m_type;
        protected: GPUShader(GPUShaderType type, char* buffer, int size);
        public: virtual ~GPUShader();

        public: // GPU Shader Attributes
            virtual bool checkCompile();
            virtual std::string checkReport();
            GPUShaderType getType() { return m_type; };
    };

    // ----------------------------
    // GPU Objects: Program Uniform
    // ----------------------------

    class GPUUniform {
        protected:
            std::string m_label;
            GPUProgram* program;
            GPUUniform(std::string label);
        public: virtual ~GPUUniform();
        public: std::string getLabel() { return m_label; };
    };

    class GPUUniformSampler : GPUUniform {
        GPUTexture* m_texture;

        protected: GPUUniformSampler(std::string label);
        public: // GPU Sampler Attributes
            virtual void setTexture(GPUTexture* texture);
            GPUTexture *getTexture() { return m_texture; };
    };

    class GPUUniformBlock : GPUUniform {
        GPUBuffer* m_ubo;

        protected: GPUUniformBlock(std::string label);
        public: // GPU Sampler Attributes
            virtual void setBuffer(GPUBuffer* m_ubo);
            GPUBuffer *getBuffer() { return m_ubo; };
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
        GPUUniformValueType m_type;
        unsigned int m_value[16];

        protected: GPUUniformValue(GPUUniformValueType type, std::string label);
        public: // GPU Sampler Attributes
            virtual void setValue(void* value);
            virtual void getValue(void* value);
            GPUUniformValueType getType() { return m_type; };
            virtual int getTypeSize();
    };

    // --------------------
    // GPU Objects: Program
    // --------------------

    class GPUProgram {
        protected: GPUProgram();
        public: virtual ~GPUProgram();

        public: // GPU Program Shader Attachment
            virtual void attachVertex(GPUShader* vertex);
            virtual void attachFragment(GPUShader* fragment);
            virtual void attachCompute(GPUShader* compute);
            virtual bool compileProgram();
            virtual std::string compileReport();

        public: // GPU Program Uniforms
            virtual GPUUniformValue* uniformValue(std::string label);
            virtual GPUUniformSampler* uniformSampler(std::string label);
            virtual GPUUniformBlock* uniformBlock(std::string label);

        friend GPUShader;
        friend GPUUniform;
        friend GPUUniformSampler;
        friend GPUUniformBlock;
        friend GPUUniformValue;
    };

    // -----------
    // GPU Context
    // -----------

    class GPUContext {
        static GPUContext* m_current;

        protected: GPUContext();
        public: virtual ~GPUContext();

        public: // GPU Objects Creation
            virtual void makeCurrent();
            virtual GPUBuffer* createBuffer(GPUBufferTarget m_target);
            virtual GPUVertexArray* createVertexArray();
            virtual GPUTexture2D* createTexture2D();
            virtual GPUTexture3D* createTexture3D();
            virtual GPUTextureCubemap* createTextureCubemap();
            virtual GPURenderbuffer* createRenderbuffer(int w, int h, GPUTexturePixelFormat format, int msaa_samples = 0);
            virtual GPUFramebuffer* createFramebuffer();
            virtual GPUShader* createShader(GPUShaderType type, char* buffer, int size);
            virtual GPUProgram* createProgram();

        public: // GPU Objects Make Current
            virtual void useBuffer(GPUBuffer *buffer);
            virtual void useVertexArray(GPUVertexArray *vertex_array);
            virtual void useTexture(GPUTexture *texture, int index);
            virtual void useFramebuffer(GPUFramebuffer* draw);
            virtual void useFramebuffer(GPUFramebuffer* draw, GPUFramebuffer* read);
            virtual void useFramebufferContext();
            virtual void useProgram(GPUProgram *program);
        private: // GPU Object Bindings Cache
            GPUBuffer *m_buffers[16];
            GPUTexture *m_textures[64];
            GPUVertexArray *m_vertex_array;
            GPURenderbuffer *m_renderbuffer;
            GPUFramebuffer *m_framebuffer_read;
            GPUFramebuffer *m_framebuffer_draw;
            GPUProgram *m_program;

        // GPU Objects Friend
        friend GPUDriver;
        friend GPUBuffer;
        friend GPUVertexArray;
        // GPU Objects Friend: Texture
        friend GPUTexture;
        friend GPUTexture2D;
        friend GPUTexture3D;
        friend GPUTextureCubemap;
        // GPU Objects Friend: Shader
        friend GPURenderbuffer;
        friend GPUFramebuffer;
        friend GPUShader;
        friend GPUUniform;
        friend GPUProgram;
    };

#endif // NOGPU_H
