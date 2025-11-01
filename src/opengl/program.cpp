// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/program.h"
#include "private/context.h"
#include <cstdlib>

GLProgram::GLProgram(GLContext* ctx) {
    ctx->makeCurrent(this);
    // Create OpenGL Program
    m_uniforms = GPUHashmap<GLUniform*>();
    m_status = GLProgramStatus::STATUS_NONE;
    m_program = glCreateProgram();
    m_report = nullptr;
    m_ctx = ctx;
}

void GLProgram::destroy() {
    m_ctx->makeCurrent(this);
    for (GLUniform* uniform : m_uniforms)
        uniform->destroy();

    // Destroy Object
    glUseProgram(0);
    glDeleteProgram(m_program);
    delete this;
}

// ----------------------
// OpenGL Program Shaders
// ----------------------

void GLProgram::attachShader(GPUShader *shader, GPUShaderType type, const char* msg) {
    m_ctx->makeCurrent(this);
    if (m_status != GLProgramStatus::STATUS_NONE) {
        GPUReport::error("program is already compiled");
        return;
    }

    // Check OpenGL Shader Type
    GLShader* s = static_cast<GLShader*>(shader);
    if (s->m_type != type) {
        GPUReport::error("%s", msg);
        return;
    }

    // Attach Shader to Program
    glUseProgram(m_program);
    glAttachShader(m_program, s->m_shader);
}

void GLProgram::attachVertex(GPUShader *vertex) {
    this->attachShader(vertex, GPUShaderType::SHADER_VERTEX, "shader is not vertex type");
}

void GLProgram::attachFragment(GPUShader *fragment) {
    this->attachShader(fragment, GPUShaderType::SHADER_FRAGMENT, "shader is not fragment type");
}

void GLProgram::attachCompute(GPUShader *compute) {
    this->attachShader(compute, GPUShaderType::SHADER_COMPUTE, "shader is not compute type");
}

// -------------------------------
// OpenGL Program Shaders: Compile
// -------------------------------

bool GLProgram::compileProgram() {
    m_ctx->makeCurrent(this);
    switch (m_status) {
        case GLProgramStatus::STATUS_NONE: break;
        case GLProgramStatus::STATUS_COMPILED: return true;
        case GLProgramStatus::STATUS_ERROR: return false;
    }

    glUseProgram(m_program);
    glLinkProgram(m_program);
    // Check if Properly Compiled
    GLint status = GL_FALSE;
    glGetProgramiv(m_program, GL_LINK_STATUS, &status);
    m_status = (status == GL_TRUE) ?
        GLProgramStatus::STATUS_COMPILED :
        GLProgramStatus::STATUS_ERROR;

    // Return Complie Check
    return status == GL_TRUE;
}

const char* GLProgram::compileReport() {
    m_ctx->makeCurrent(this);
    if (m_status != GLProgramStatus::STATUS_ERROR)
        return nullptr;

    // Get Program Status
    glUseProgram(m_program);
    // Check Report Size
    GLint report_bytes = 0;
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &report_bytes);
    if (report_bytes <= 0)
        return nullptr;

    if (m_report != nullptr)
        free(m_report);
    // Output Program Reporting
    m_report = (char*) malloc(report_bytes + 1);
    glGetProgramInfoLog(m_program, report_bytes + 1, &report_bytes, m_report);
    m_report[report_bytes] = '\0';
    return m_report;
}

// -----------------------
// OpenGL Program Uniforms
// -----------------------

GPUUniform* GLProgram::createUniform(const char* label, GPUUniformType type) {
    m_ctx->makeCurrent(this);
    if (m_status != GLProgramStatus::STATUS_COMPILED) {
        GPUReport::error("program is not compiled");
        return nullptr;
    }

    glUseProgram(m_program);
    // Check Not yet Created
    GLUniform* uniform = nullptr;
    GLUniform** found = m_uniforms.get_name(label);
    if (found == nullptr) {
        bool check = false;
        uniform = new GLUniform(this, type, label, check);
        if (check == false)
            return nullptr;

        // Add Uniform to Hashmap
        m_uniforms.add_name(label, uniform);
        return uniform;
    }

    // Check Collided Uniform
    uniform = *found;
    if (uniform->m_type != type) {
        GPUReport::error("mismatch type with collided uniform: %s", label);
        return nullptr;
    }

    // Return Cached Uniform
    return uniform;
}

GPUUniform* GLProgram::getUniform(const char* label) {
    m_ctx->makeCurrent(this);
    if (m_status != GLProgramStatus::STATUS_COMPILED) {
        GPUReport::error("program is not compiled");
        return nullptr;
    }

    glUseProgram(m_program);
    GLUniform** found = m_uniforms.get_name(label);
    if (found == nullptr) {
        GPUReport::warning("uniform not found: %s", label);
        return nullptr;
    }

    // Return Uniform
    return *found;
}

bool GLProgram::removeUniform(const char* label) {
    m_ctx->makeCurrent(this);
    if (m_status != GLProgramStatus::STATUS_COMPILED) {
        GPUReport::error("program is not compiled");
        return false;
    }

    glUseProgram(m_program);
    GLUniform** found = m_uniforms.get_name(label);
    if (found == nullptr) {
        GPUReport::warning("uniform not found: %s", label);
        return false;
    }

    // Remove Uniform
    delete found[0];
    m_uniforms.remove_name(label);
    return true;
}
