// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H
#include <nogpu_map.h>
#include <nogpu/shader.h>
#include "glad.h"

// -----------------
// OpenGL GPU Shader
// -----------------

class GLProgram;
class GLContext;
class GLShader : public GPUShader {
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
        friend GLProgram;
};

// ------------------
// OpenGL GPU Uniform
// ------------------

class GLUniform : public GPUUniform {
    unsigned int m_value[16];
    GLProgram* m_program;
    GLContext* m_ctx;
    GLuint m_uniform;
    GPUUniformType m_type;

    // GPU Sampler Attributes: Value
    void setValueRaw(void *data) override;
    void setValueBoolean(bool value) override;
    void setValueInteger(int value) override;
    void setValueFloat(float value) override;
    // GPU Sampler Attributes: Sampler
    void setTextureSampler(int index) override;
    void setBlockBufferObject(int index) override;
    void setBlockShaderStorage(int index) override;
    void setBlockAtomicCounter(int index) override;

    // GPU Sampler Attributes: Getter
    GPUProgram* getProgram() override;
    GPUUniformType getType() override;
    void getValue(void *output) override;
    int getBytes() override;

    protected: // OpenGL Uniform Constructor
        GLUniform(GLProgram* program, GPUUniformType type, const char* name, bool &result);
        void setBlockIndex(int index, GPUUniformType type, const char* msg);
        void destroy();
        friend GLProgram;
        friend GLContext;
};

// ------------------
// OpenGL GPU Program
// ------------------

enum class GLProgramStatus : int {
    STATUS_NONE,
    STATUS_COMPILED,
    STATUS_ERROR
};

class GLProgram : public GPUProgram {
    GPUHashmap<GLUniform*> m_uniforms;
    GLContext* m_ctx;
    GLProgramStatus m_status;
    GLuint m_program;
    char* m_report;

    // GPU Program Shaders
    void attachVertex(GPUShader *vertex) override;
    void attachFragment(GPUShader *fragment) override;
    void attachCompute(GPUShader *compute) override;
    bool compileProgram() override;
    const char* compileReport() override;

    // GPU Program Uniforms
    GPUUniform* createUniform(const char* label, GPUUniformType type) override;
    GPUUniform* getUniform(const char* label) override;
    bool removeUniform(const char* label) override;

    protected: // OpenGL Program
        GLProgram(GLContext* ctx);
        void destroy() override;
        void attachShader(GPUShader *shader, GPUShaderType type, const char* msg);
        friend GLContext;
        friend GLUniform;
        friend GLShader;
};

#endif // OPENGL_SHADER_H
