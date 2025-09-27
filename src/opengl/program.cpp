// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "private/shader.h"
#include "private/context.h"

GLProgram::GLProgram(GLContext* ctx) {

}

void GLProgram::destroy() {

}

// ----------------------
// OpenGL Program Shaders
// ----------------------

void GLProgram::attachVertex(GPUShader *vertex) {

}

void GLProgram::attachFragment(GPUShader *fragment) {

}

void GLProgram::attachCompute(GPUShader *compute) {

}

bool GLProgram::compileProgram() {

}

const char* GLProgram::compileReport() {

}

// -----------------------
// OpenGL Program Uniforms
// -----------------------

GPUUniform* GLProgram::createUniform(const char* label, GPUUniformType type) {

}

void GLProgram::removeUniformObject(GPUUniform* uniform) {

}

void GLProgram::removeUniformLabel(const char* label) {

}
