// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_SHADER_H
#define NOGPU_SHADER_H
#include "buffer.h"
#include "texture.h"

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
    protected: ~GPUShader();
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
        ~GPUUniform();
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
    protected:
        GPUProgram();
        ~GPUProgram();
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
        virtual GPUUniform *removeUniform(char* label) = 0;
        virtual GPUUniform *getUniform(char* label) = 0;
};

#endif // NOGPU_SHADER_H
