// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H
#include <nogpu/shader.h>

// -----------------
// OpenGL GPU Shader
// -----------------

class GLContext;
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

#endif // OPENGL_SHADER_H
