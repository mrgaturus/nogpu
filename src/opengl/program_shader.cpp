// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/program.h"
#include "private/context.h"
#include <cstdlib>

static void compileShaderGLSL(GLuint shader, GPUShaderSource &data) {
    glShaderSource(shader, 1, &data.buffer, &data.buffer_bytes);
    glCompileShader(shader);
}

static void compileShaderSPIR_V(GLuint shader, GPUShaderSource &data) {
    glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V, data.buffer, data.buffer_bytes);
    if (data.entrypoint == nullptr)
        data.entrypoint = "main";

    glSpecializeShaderARB(shader, data.entrypoint,
        data.specialized_count,
        data.specialized_indices,
        data.specialized_constants
    );
}

// -----------------------
// OpenGL Shader: Creation
// -----------------------

GLShader::GLShader(GLContext* ctx, GPUShaderType type, GPUShaderSource data) {
    ctx->makeCurrent(this);
    GLuint shader = 0;

    // Create Shader Object
    switch (type) {
        case GPUShaderType::SHADER_VERTEX:
            shader = glCreateShader(GL_VERTEX_SHADER);
        case GPUShaderType::SHADER_FRAGMENT:
            shader = glCreateShader(GL_FRAGMENT_SHADER);
        case GPUShaderType::SHADER_COMPUTE:
            shader = glCreateShader(GL_COMPUTE_SHADER);
    }

    // Compile Shader Program
    switch (data.driver) {
        case GPUShaderDriver::SHADER_GLSL:
            compileShaderGLSL(shader, data);
            break;
        case GPUShaderDriver::SHADER_SPIRV:
            if (GLAD_GL_ARB_gl_spirv) {
                compileShaderSPIR_V(shader, data);
                break;
            }

        default: // Unsupported Shader
            GPUReport::error("unsupported shader driver");
            delete this;
            return;
    }

    // Define Shader Object
    m_type = type;
    m_driver = data.driver;
    m_shader = shader;
    m_ctx = ctx;
}

void GLShader::destroy() {
    m_ctx->makeCurrent(this);
    glDeleteShader(m_shader);
    if (m_report != nullptr)
        free(m_report);

    // Dealloc Shader Object
    delete this;
}

// --------------------
// OpenGL Shader: Check
// --------------------

bool GLShader::compileCheck() {
    m_ctx->makeCurrent(this);
    GLint shader_status = GL_FALSE;

    // Check Shader Compilation
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &shader_status);
    return shader_status == GL_TRUE;
}

const char* GLShader::compileReport() {
    m_ctx->makeCurrent(this);
    if (compileCheck())
        return nullptr;

    // Check Report Size
    GLint report_bytes = 0;
    glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &report_bytes);
    if (report_bytes <= 0)
        return nullptr;

    if (m_report != nullptr)
        free(m_report);
    // Output Shader Reporting
    m_report = (char*) malloc(report_bytes + 1);
    glGetShaderInfoLog(m_shader, report_bytes + 1, &report_bytes, m_report);
    m_report[report_bytes] = '\0';
    return m_report;
}

// -------------------------
// OpenGL Shader: Properties
// -------------------------

GPUShaderType GLShader::getType() {
    m_ctx->makeCurrent(this);
    return m_type;
}

GPUShaderDriver GLShader::getDriver() {
    m_ctx->makeCurrent(this);
    return m_driver;
}
