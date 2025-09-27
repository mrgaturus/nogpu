// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "private/shader.h"
#include "private/context.h"

GLUniform::GLUniform(GLProgram* program, unsigned int name, GPUUniformType type) {

}

void GLUniform::destroy() {

}

// --------------------------------
// OpenGL Sampler Attributes: Value
// --------------------------------

void GLUniform::setValueRaw(void *src) {

}

void GLUniform::setValueBoolean(bool value) {

}

void GLUniform::setValueInteger(int value) {

}

void GLUniform::setValueFloat(float value) {

}


// ---------------------------------
// OpenGL Sampler Attributes: Sampler
// ---------------------------------

void GLUniform::setBlockSampler(int index) {

}

void GLUniform::setBlockBuffer(int index) {

}

void GLUniform::setBlockShaderStorage(int index) {

}

void GLUniform::setBlockAtomicCounter(int index) {

}


// ---------------------------------
// OpenGL Sampler Attributes: Getter
// ---------------------------------

GPUProgram* GLUniform::getProgram() {

}

GPUUniformType GLUniform::getType() {

}

void GLUniform::getValue(void *dest) {

}

int GLUniform::getBytes() {

}
