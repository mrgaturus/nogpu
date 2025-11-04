// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/program.h"
#include "private/context.h"
#include "private/glad.h"
#include <cstring>

GLUniform::GLUniform(GLProgram* program, GPUUniformType type, const char* name, bool &result) {
    program->m_ctx->makeCurrent(this);
    GLuint pro = program->m_program;
    GLuint uniform = GL_INVALID_INDEX;
    // Create Program Uniform
    switch (type) {
        case GPUUniformType::UNIFORM_BLOCK_BUFFER_OBJECT:
            if (!GLAD_GL_ARB_uniform_buffer_object) {
                GPUReport::error("uniform buffer object not supported");
                goto FAILED_UNIFORM;
            }
            // Lookup Block Uniform
            uniform = glGetUniformBlockIndex(pro, name);
            break;
        case GPUUniformType::UNIFORM_BLOCK_SHADER_STORAGE:
            if (!GLAD_GL_ARB_shader_storage_buffer_object) {
                GPUReport::error("uniform shader storage not supported");
                goto FAILED_UNIFORM;
            }
            // Lookup Block Uniform
            uniform = glGetUniformBlockIndex(pro, name);
            break;
        case GPUUniformType::UNIFORM_BLOCK_ATOMIC_COUNTER:
            if (!GLAD_GL_ARB_shader_atomic_counters) {
                GPUReport::error("uniform atomic counters not supported");
                goto FAILED_UNIFORM;
            }
            // Lookup Block Uniform
            uniform = glGetUniformBlockIndex(pro, name);
            break;
        
        case GPUUniformType::UNIFORM_UNSIGNED_INT:
        case GPUUniformType::UNIFORM_UNSIGNED_INT_x2:
        case GPUUniformType::UNIFORM_UNSIGNED_INT_x3:
        case GPUUniformType::UNIFORM_UNSIGNED_INT_x4:
            if (glUniform1uiv == nullptr) {
                GPUReport::error("unsigned int uniform is not supported");
                goto FAILED_UNIFORM;
            }

        default:
            uniform = glGetUniformLocation(pro, name);
            break;
    }

    // Check Valid Uniform
    result = uniform != GL_INVALID_INDEX;
    if (uniform != GL_INVALID_INDEX) {
        GPUReport::error("uniform not available in program: %s", name);
        delete this;
    }

// Create Uniform Object
SUCCESS_UNIFORM:
    memset(m_value, 0, sizeof(m_value));
    m_program = program;
    m_ctx = program->m_ctx;
    m_uniform = uniform;
    m_type = type;
    result = true;
    return;

FAILED_UNIFORM:
    result = false;
    delete this;
    return;
}

void GLUniform::destroy() {
    m_ctx->makeCurrent(this);
    // Clear Shader Value
    memset(m_value, 0, sizeof(m_value));
    glUseProgram(m_program->m_program);
    delete this;
}

// ----------------------------------------
// OpenGL Sampler Attributes: Value General
// ----------------------------------------

void GLUniform::setValueRaw(void *data) {
    m_ctx->makeCurrent(this);
    const int bytes = computeUniformBytes(m_type);
    // Copy Value to Cache
    void* cache = (void*) m_value;
    memcpy(cache, data, bytes);
    GLuint program = m_program->m_program;
    GLuint uniform = m_uniform;
    glUseProgram(program);

    // Copy Value to Shader
    switch (m_type) {
        // Uniform Texture Sampler
       case GPUUniformType::UNIFORM_TEXTURE_SAMPLER:
            glUniform1iv(uniform, 1, (const GLint*) cache);
            return;
        // Uniform Buffer Blocks
        case GPUUniformType::UNIFORM_BLOCK_BUFFER_OBJECT:
        case GPUUniformType::UNIFORM_BLOCK_SHADER_STORAGE:
        case GPUUniformType::UNIFORM_BLOCK_ATOMIC_COUNTER: {
            GLuint binding = ((const GLuint*) cache)[0];
            glUniformBlockBinding(program, uniform, binding);
            return;
        }

        // Integers & Floats x1
        case GPUUniformType::UNIFORM_BOOL:
        case GPUUniformType::UNIFORM_INT:
            glUniform1iv(uniform, 1, (const GLint*) cache);
            return;
        case GPUUniformType::UNIFORM_FLOAT:
            glUniform1fv(uniform, 1, (const GLfloat*) cache);
            return;
        case GPUUniformType::UNIFORM_UNSIGNED_INT:
            glUniform1uiv(uniform, 1, (const GLuint*) cache);
            return;

        // Integers & Floats x2
        case GPUUniformType::UNIFORM_BOOL_x2:
        case GPUUniformType::UNIFORM_INT_x2:
            glUniform2iv(uniform, 1, (const GLint*) cache);
            return;
        case GPUUniformType::UNIFORM_FLOAT_x2:
            glUniform2fv(uniform, 1, (const GLfloat*) cache);
            return;
        case GPUUniformType::UNIFORM_UNSIGNED_INT_x2:
            glUniform2uiv(uniform, 1, (const GLuint*) cache);
            return;

        // Integers & Floats x3
        case GPUUniformType::UNIFORM_BOOL_x3:
        case GPUUniformType::UNIFORM_INT_x3:
            glUniform3iv(uniform, 1, (const GLint*) cache);
            return;
        case GPUUniformType::UNIFORM_FLOAT_x3:
            glUniform3fv(uniform, 1, (const GLfloat*) cache);
            return;
        case GPUUniformType::UNIFORM_UNSIGNED_INT_x3:
            glUniform3uiv(uniform, 1, (const GLuint*) cache);
            return;

        // Integers & Floats x4
        case GPUUniformType::UNIFORM_BOOL_x4:
        case GPUUniformType::UNIFORM_INT_x4:
            glUniform4iv(uniform, 1, (const GLint*) cache);
            return;
        case GPUUniformType::UNIFORM_FLOAT_x4:
            glUniform4fv(uniform, 1, (const GLfloat*) cache);
            return;
        case GPUUniformType::UNIFORM_UNSIGNED_INT_x4:
            glUniform4uiv(uniform, 1, (const GLuint*) cache);
            return;

        // Uniform Matrix - Square
        case GPUUniformType::UNIFORM_MATRIX_2x2:
            glUniformMatrix2fv(uniform, 1, false, (const GLfloat*) cache);
            return;
        case GPUUniformType::UNIFORM_MATRIX_3x3:
            glUniformMatrix3fv(uniform, 1, false, (const GLfloat*) cache);
            return;
        case GPUUniformType::UNIFORM_MATRIX_4x4:
            glUniformMatrix4fv(uniform, 1, false, (const GLfloat*) cache);
            return;
        // Uniform Matrix - 2x3 / 3x2
        case GPUUniformType::UNIFORM_MATRIX_2x3:
            glUniformMatrix2x3fv(uniform, 1, false, (const GLfloat*) cache);
            return;
        case GPUUniformType::UNIFORM_MATRIX_3x2:
            glUniformMatrix3x2fv(uniform, 1, false, (const GLfloat*) cache);
            return;
        // Uniform Matrix - 2x4 / 4x2
        case GPUUniformType::UNIFORM_MATRIX_2x4:
            glUniformMatrix2x4fv(uniform, 1, false, (const GLfloat*) cache);
            return;
        case GPUUniformType::UNIFORM_MATRIX_4x2:
            glUniformMatrix4x2fv(uniform, 1, false, (const GLfloat*) cache);
            return;
        // Uniform Matrix - 3x4 / 4x3
        case GPUUniformType::UNIFORM_MATRIX_3x4:
            glUniformMatrix3x4fv(uniform, 1, false, (const GLfloat*) cache);
            return;
        case GPUUniformType::UNIFORM_MATRIX_4x3:
            glUniformMatrix4x3fv(uniform, 1, false, (const GLfloat*) cache);
            return;

        // Uniform Matrix Transposed - Square
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_2x2:
            glUniformMatrix2fv(uniform, 1, true, (const GLfloat*) cache);
            return;
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_3x3:
            glUniformMatrix3fv(uniform, 1, true, (const GLfloat*) cache);
            return;
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_4x4:
            glUniformMatrix4fv(uniform, 1, true, (const GLfloat*) cache);
            return;
        // Uniform Matrix Transposed - 2x3 / 3x2
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_2x3:
            glUniformMatrix2x3fv(uniform, 1, true, (const GLfloat*) cache);
            return;
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_3x2:
            glUniformMatrix3x2fv(uniform, 1, true, (const GLfloat*) cache);
            return;
        // Uniform Matrix Transposed - 2x4 / 4x2
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_2x4:
            glUniformMatrix2x4fv(uniform, 1, true, (const GLfloat*) cache);
            return;
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_4x2:
            glUniformMatrix4x2fv(uniform, 1, true, (const GLfloat*) cache);
            return;
        // Uniform Matrix Transposed - 3x4 / 4x3
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_3x4:
            glUniformMatrix3x4fv(uniform, 1, true, (const GLfloat*) cache);
            return;
        case GPUUniformType::UNIFORM_MATRIX_TRANSPOSED_4x3:
            glUniformMatrix4x3fv(uniform, 1, true, (const GLfloat*) cache);
            return;
    }
}

// --------------------------------------
// OpenGL Sampler Attributes: Value Quick
// --------------------------------------

void GLUniform::setValueBoolean(bool value) {
    m_ctx->makeCurrent(this);
    glUseProgram(m_program->m_program);
    GLint v = value;

    switch (m_type) {
        case GPUUniformType::UNIFORM_BOOL:
        case GPUUniformType::UNIFORM_INT:
            v = value ? GL_TRUE : GL_FALSE;
            glUniform1i(m_uniform, v);
            m_value[0] = static_cast<unsigned int>(v);
            break;

        default:
            GPUReport::error("uniform is not UNIFORM_BOOL or UNIFORM_INT");
            break;
    }
}

void GLUniform::setValueInteger(int value) {
    m_ctx->makeCurrent(this);
    glUseProgram(m_program->m_program);
    // Check if is Uniform Integer
    if (m_type != GPUUniformType::UNIFORM_INT) {
        GPUReport::error("uniform is not UNIFORM_INT");
        return;
    }

    // Set Uniform Integer Value
    glUniform1i(m_uniform, value);
    m_value[0] = value;
}

void GLUniform::setValueFloat(float value) {
    m_ctx->makeCurrent(this);
    glUseProgram(m_program->m_program);
    // Check if is Uniform Float
    if (m_type != GPUUniformType::UNIFORM_FLOAT) {
        GPUReport::error("uniform is not UNIFORM_FLOAT");
        return;
    }

    // Set Uniform Float Value
    glUniform1f(m_uniform, value);
    float *v = reinterpret_cast<float*>(m_value);
    v[0] = value;
}

// ----------------------------------------
// OpenGL Sampler Attributes: Block Binding
// ----------------------------------------

void GLUniform::setTextureSampler(int index) {
    m_ctx->makeCurrent(this);
    glUseProgram(m_program->m_program);
    // Check if is Uniform Texture
    if (m_type != GPUUniformType::UNIFORM_TEXTURE_SAMPLER) {
        GPUReport::error("uniform is not UNIFORM_TEXTURE_SAMPLER");
        return;
    }

    // Set Texture Sampler
    glUniform1i(m_uniform, index);
    m_value[0] = index;
}


void GLUniform::setBlockIndex(int index, GPUUniformType type, const char* msg) {
    m_ctx->makeCurrent(this);
    GLuint program = m_program->m_program;
    glUseProgram(program);
    // Check if is Uniform Block
    if (m_type != type) {
        GPUReport::error("%s", msg);
        return;
    }

    // Set Texture Block
    glUniformBlockBinding(program, m_uniform, index);
    m_value[0] = index;
}

void GLUniform::setBlockBufferObject(int index) {
    this->setBlockIndex(index, GPUUniformType::UNIFORM_BLOCK_BUFFER_OBJECT,
        "uniform is not UNIFORM_BLOCK_BUFFER_OBJECT");
}

void GLUniform::setBlockShaderStorage(int index) {
    this->setBlockIndex(index, GPUUniformType::UNIFORM_BLOCK_SHADER_STORAGE,
        "uniform is not UNIFORM_BLOCK_SHADER_STORAGE");
}

void GLUniform::setBlockAtomicCounter(int index) {
    this->setBlockIndex(index, GPUUniformType::UNIFORM_BLOCK_ATOMIC_COUNTER,
        "uniform is not UNIFORM_BLOCK_ATOMIC_COUNTER");
}

// ---------------------------------
// OpenGL Sampler Attributes: Getter
// ---------------------------------

GPUProgram* GLUniform::getProgram() {
    m_ctx->makeCurrent(this);
    return m_program;
}

GPUUniformType GLUniform::getType() {
    m_ctx->makeCurrent(this);
    return m_type;
}

void GLUniform::getValue(void *output) {
    m_ctx->makeCurrent(this);
    // Copy Value Data to Destination
    const int bytes = computeUniformBytes(m_type);
    memcpy(output, m_value, bytes);
}

int GLUniform::getBytes() {
    m_ctx->makeCurrent(this);
    return computeUniformBytes(m_type);
}
