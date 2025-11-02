// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "private/pipeline.h"
#include "private/context.h"
#include "private/program.h"

GLPipeline::GLPipeline(GLContext* ctx) {
    ctx->makeCurrent(this);
    m_state = GLPipelineState{};
    m_ctx_state = &ctx->m_device->m_state;
    m_ctx = ctx;
}

void GLPipeline::markEffect(GLPipelineEffect effect) {
    if (m_ctx_state->m_pipeline_current == this)
        m_ctx_state->markPipelineEffect(effect);
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
    markEffect(GLPipelineEffect::PIPELINE_EFFECT_CAPABILITIES);
    m_state.capabilities |= 1 << static_cast<unsigned int>(cap);
}

void GLPipeline::disableCapability(GPUPipelineCapability cap) {
    m_ctx->makeCurrent(this);
    markEffect(GLPipelineEffect::PIPELINE_EFFECT_CAPABILITIES);
    m_state.capabilities &= ~(1 << static_cast<unsigned int>(cap));
}

// --------------------------
// GPU Pipeline State: Basics
// --------------------------

void GLPipeline::setProgram(GPUProgram *program) {
    m_ctx->makeCurrent(this);
    markEffect(GLPipelineEffect::PIPELINE_EFFECT_PROGRAM);
    GLProgram* pro = static_cast<GLProgram*>(program);
    m_state.program = pro;
}

void GLPipeline::setBlending(GPUPipelineBlending blending) {
    m_ctx->makeCurrent(this);
    markEffect(GLPipelineEffect::PIPELINE_EFFECT_BLENDING);
    m_state.blending = blending;
}

void GLPipeline::setCulling(GPUPipelineFace face) {
    m_ctx->makeCurrent(this);
    markEffect(GLPipelineEffect::PIPELINE_EFFECT_CULLING);
    m_state.culling = face;
}

void GLPipeline::setDepth(GPUPipelineDepth depth) {
    m_ctx->makeCurrent(this);
    markEffect(GLPipelineEffect::PIPELINE_EFFECT_DEPTH);
    m_state.depth = depth;
}

void GLPipeline::setStencil(GPUPipelineStencil stencil) {
    m_ctx->makeCurrent(this);
    markEffect(GLPipelineEffect::PIPELINE_EFFECT_STENCIL);
    m_state.stencil = stencil;
}

// ----------------------------
// GPU Pipeline State: Viewport
// ----------------------------

void GLPipeline::setClearDepth(float depth) {
    m_ctx->makeCurrent(this);
    markEffect(GLPipelineEffect::PIPELINE_EFFECT_CLEAR_DEPTH);
    m_state.clear_depth = depth;
}

void GLPipeline::setClearStencil(int mask) {
    m_ctx->makeCurrent(this);
    markEffect(GLPipelineEffect::PIPELINE_EFFECT_CLEAR_STENCIL);
    m_state.clear_stencil = mask;
}

void GLPipeline::setClearColor(GPUColor color) {
    m_ctx->makeCurrent(this);
    markEffect(GLPipelineEffect::PIPELINE_EFFECT_CLEAR_COLOR);
    m_state.clear_color = color;
}

void GLPipeline::setColorMask(GPUColorMask mask) {
    m_ctx->makeCurrent(this);
    markEffect(GLPipelineEffect::PIPELINE_EFFECT_COLOR_MASK);
    m_state.color_mask = mask;
}

void GLPipeline::setViewport(GPURectangle rect) {
    m_ctx->makeCurrent(this);
    markEffect(GLPipelineEffect::PIPELINE_EFFECT_VIEWPORT);
    m_state.viewport = rect;
}

void GLPipeline::setScissor(GPURectangle rect) {
    m_ctx->makeCurrent(this);
    markEffect(GLPipelineEffect::PIPELINE_EFFECT_SCISSOR);
    m_state.scissor = rect;
}

void GLPipeline::setLineWidth(float width) {
    m_ctx->makeCurrent(this);
    markEffect(GLPipelineEffect::PIPELINE_EFFECT_LINE_WIDTH);
    m_state.line_width = width;
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
    return m_state.color_mask;
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
