// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "private/pipeline.h"
#include "private/context.h"
#include "private/shader.h"

GLPipeline::GLPipeline(GLContext* ctx) {
    ctx->makeCurrent(this);
    m_ctx = ctx;
}

void GLPipeline::destroy() {
    m_ctx->makeCurrent(this);
    delete this;
}

// ---------------------------
// OpenGL Pipeline Capabilites
// ---------------------------

bool GLPipeline::checkCapability(GPUPipelineCapability cap) {
    m_ctx->makeCurrent(this);
    // Check Activated Capability
    unsigned int flag = (1 << static_cast<unsigned int>(cap));
    return (m_state.capabilities & flag) != 0;
}

void GLPipeline::enableCapability(GPUPipelineCapability cap) {
    m_ctx->makeCurrent(this);
}

void GLPipeline::disableCapability(GPUPipelineCapability cap) {
    m_ctx->makeCurrent(this);
}

// --------------------------
// GPU Pipeline State: Basics
// --------------------------

void GLPipeline::setProgram(GPUProgram *program) {
    m_ctx->makeCurrent(this);
}

void GLPipeline::setBlending(GPUPipelineBlending blending) {
    m_ctx->makeCurrent(this);
}

void GLPipeline::setCulling(GPUPipelineFace face) {
    m_ctx->makeCurrent(this);
}

void GLPipeline::setDepth(GPUPipelineDepth depth) {
    m_ctx->makeCurrent(this);
}

void GLPipeline::setStencil(GPUPipelineStencil stencil) {
    m_ctx->makeCurrent(this);
}

// ----------------------------
// GPU Pipeline State: Viewport
// ----------------------------

void GLPipeline::setClearDepth(float depth) {
    m_ctx->makeCurrent(this);
}

void GLPipeline::setClearStencil(int mask) {
    m_ctx->makeCurrent(this);
}

void GLPipeline::setClearColor(GPUColor color) {
    m_ctx->makeCurrent(this);
}

void GLPipeline::setColorMask(GPUColorMask mask) {
    m_ctx->makeCurrent(this);
}

void GLPipeline::setViewport(GPURectangle rect) {
    m_ctx->makeCurrent(this);
}

void GLPipeline::setScissor(GPURectangle rect) {
    m_ctx->makeCurrent(this);
}

void GLPipeline::setLineWidth(float width) {
    m_ctx->makeCurrent(this);
}

// ----------------------------
// GPU Pipeline Getters: Basics
// ----------------------------

GPUProgram* GLPipeline::getProgram() {
    m_ctx->makeCurrent(this);
    return m_state.program;
}

GPUPipelineBlending GLPipeline::getBlending() {
    m_ctx->makeCurrent(this);
    return m_state.blending;
}

GPUPipelineFace GLPipeline::getCulling() {
    m_ctx->makeCurrent(this);
    return m_state.culling;
}

GPUPipelineDepth GLPipeline::getDepth() {
    m_ctx->makeCurrent(this);
    return m_state.depth;
}

GPUPipelineStencil GLPipeline::getStencil() {
    m_ctx->makeCurrent(this);
    return m_state.stencil;
}


// ------------------------------
// GPU Pipeline Getters: Viewport
// ------------------------------

float GLPipeline::getClearDepth() {
    m_ctx->makeCurrent(this);
    return m_state.clear_depth;
}

int GLPipeline::getClearStencil() {
    m_ctx->makeCurrent(this);
    return m_state.clear_stencil;
}

GPUColor GLPipeline::getClearColor() {
    m_ctx->makeCurrent(this);
    return m_state.clear_color;
}

GPUColorMask GLPipeline::getColorMask() {
    m_ctx->makeCurrent(this);
    return m_state.mask_color;
}

GPURectangle GLPipeline::getViewport() {
    m_ctx->makeCurrent(this);
    return m_state.viewport;
}

GPURectangle GLPipeline::getScissor() {
    m_ctx->makeCurrent(this);
    return m_state.scissor;
}

float GLPipeline::getLineWidth() {
    m_ctx->makeCurrent(this);
    return m_state.line_width;
}
