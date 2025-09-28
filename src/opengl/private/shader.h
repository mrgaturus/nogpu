// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H
#include <nogpu/shader.h>
#include "glad.h"

// -----------------
// OpenGL GPU Shader
// -----------------

class GLContext;
class GLShader : GPUShader {
    GLContext* m_ctx;
    GLuint m_shader;
    GPUShaderType m_type;
    GPUShaderDriver m_driver;
    char* m_report;

    // GPU Shader: Check
    bool compileCheck() override;
    const char* compileReport() override;
    // GPU Shader: Properties
    GPUShaderType getType() override;
    GPUShaderDriver getDriver() override;

    protected: // OpenGL Shader Constructor
        GLShader(GLContext* ctx, GPUShaderType type, GPUShaderSource data);
        void destroy() override;
        friend GLContext;
};

// ------------------
// OpenGL GPU Uniform
// ------------------

class GLProgram;
class GLUniform : GPUUniform {
    unsigned int m_value[16];
    GLProgram* m_program;
    GLContext* m_ctx;
    GLuint m_uniform;
    unsigned int m_name;

    // GPU Sampler Attributes: Value
    void setValueRaw(void *src) override;
    void setValueBoolean(bool value) override;
    void setValueInteger(int value) override;
    void setValueFloat(float value) override;

    // GPU Sampler Attributes: Sampler
    void setBlockSampler(int index) override;
    void setBlockBuffer(int index) override;
    void setBlockShaderStorage(int index) override;
    void setBlockAtomicCounter(int index) override;

    // GPU Sampler Attributes: Getter
    GPUProgram* getProgram() override;
    GPUUniformType getType() override;
    void getValue(void *dest) override;
    int getBytes() override;

    protected: // OpenGL Uniform Constructor
        GLUniform(GLProgram* program, unsigned int name, GPUUniformType type);
        void destroy();
        friend GLProgram;
        friend GLContext;
};

// ------------------
// OpenGL GPU Program
// ------------------

class GLProgram : GPUProgram {
    GLContext* m_ctx;
    GLuint m_program;

    // GPU Program Shaders
    void attachVertex(GPUShader *vertex) override;
    void attachFragment(GPUShader *fragment) override;
    void attachCompute(GPUShader *compute) override;
    bool compileProgram() override;
    const char* compileReport() override;

    // GPU Program Uniforms
    GPUUniform* createUniform(const char* label, GPUUniformType type) override;
    void removeUniformObject(GPUUniform* uniform) override;
    void removeUniformLabel(const char* label) override;

    protected: // OpenGL Program Constructor
        GLProgram(GLContext* ctx);
        void destroy() override;
        friend GLContext;
};

#endif // OPENGL_SHADER_H
