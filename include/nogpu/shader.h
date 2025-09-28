// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_SHADER_H
#define NOGPU_SHADER_H

// ---------------------------
// GPU Objects: Program Shader
// ---------------------------

enum class GPUShaderType : int {
    SHADER_VERTEX,
    SHADER_FRAGMENT,
    SHADER_COMPUTE
};

enum class GPUShaderDriver : int {
    SHADER_GLSL, // OpenGL
    SHADER_SPIRV, // Vulkan
    SHADER_DXBC, // DirectX 11
    SHADER_DXIL, // DirectX 12
    SHADER_MTL, // Metal
};

typedef struct {
    GPUShaderDriver driver;
    // Shader Buffer
    int buffer_bytes;
    const char* buffer;
    const char* entrypoint;
    // Specialized Constants
    int specialized_count;
    const unsigned int* specialized_constants;
    const unsigned int* specialized_indices;
} GPUShaderSource;

class GPUShader {
    public: // GPU Shader: Check
        virtual void destroy() = 0;
        virtual bool compileCheck() = 0;
        virtual const char* compileReport() = 0;
    public: // GPU Shader: Properties
        virtual GPUShaderType getType() = 0;
        virtual GPUShaderDriver getDriver() = 0;
};

// ----------------------------
// GPU Objects: Program Uniform
// ----------------------------

enum class GPUUniformType : int {
    UNIFORM_TEXTURE_SAMPLER,
    UNIFORM_BLOCK_BUFFER_OBJECT,
    UNIFORM_BLOCK_SHADER_STORAGE,
    UNIFORM_BLOCK_ATOMIC_COUNTER,

    // Booleans
    UNIFORM_BOOL,
    UNIFORM_BOOL_x2,
    UNIFORM_BOOL_x3,
    UNIFORM_BOOL_x4,
    // Integers & Floats
    UNIFORM_INT,
    UNIFORM_INT_x2,
    UNIFORM_INT_x3,
    UNIFORM_INT_x4,
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
    UNIFORM_MATRIX_TRANSPOSED_4x3
};

class GPUProgram;
class GPUUniform {
    public: // GPU Sampler Attributes: Value
        virtual void setValueRaw(void *data) = 0;
        virtual void setValueBoolean(bool value) = 0;
        virtual void setValueInteger(int value) = 0;
        virtual void setValueFloat(float value) = 0;
    public: // GPU Sampler Attributes: Sampler
        virtual void setTextureSampler(int index) = 0;
        virtual void setBlockBufferObject(int index) = 0;
        virtual void setBlockShaderStorage(int index) = 0;
        virtual void setBlockAtomicCounter(int index) = 0;

    public: // GPU Sampler Attributes: Getter
        virtual GPUProgram* getProgram() = 0;
        virtual GPUUniformType getType() = 0;
        virtual void getValue(void *output) = 0;
        virtual int getBytes() = 0;
};

// --------------------
// GPU Objects: Program
// --------------------

class GPUProgram {
    public: // GPU Program Shaders
        virtual void destroy() = 0;
        virtual void attachVertex(GPUShader *vertex) = 0;
        virtual void attachFragment(GPUShader *fragment) = 0;
        virtual void attachCompute(GPUShader *compute) = 0;
        virtual bool compileProgram() = 0;
        virtual const char* compileReport() = 0;

    public: // GPU Program Uniforms
        virtual GPUUniform* createUniform(const char* label, GPUUniformType type) = 0;
        virtual GPUUniform* getUniform(const char* label) = 0;
        virtual bool removeUniform(const char* label) = 0;
};

#endif // NOGPU_SHADER_H
