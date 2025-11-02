// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "private/state.hpp"
#include "private/glad.h"
#include <cstring>

bool GLState::checkCapability(GPUPipelineCapability cap) {
    return (m_pipeline_state.capabilities & (1 << static_cast<unsigned int>(cap))) != 0;
}

void GLState::enableCapability(GPUPipelineCapability cap) {
    GLPipelineState *state = &m_pipeline_state;
    // Avoid Already Enabled
    unsigned int flag = 1 << static_cast<unsigned int>(cap);
    if (flag & state->capabilities)
        return;

    switch (cap) {
        case GPUPipelineCapability::CAPABILITY_BLENDING:
            glEnable(GL_BLEND);
            break;
        case GPUPipelineCapability::CAPABILITY_CULLING:
            glEnable(GL_CULL_FACE);
            break;

        case GPUPipelineCapability::CAPABILITY_DEPTH:
            glEnable(GL_DEPTH_TEST);
            break;
        case GPUPipelineCapability::CAPABILITY_DEPTH_OFFSET:
            glEnable(GL_POLYGON_OFFSET_FILL);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glEnable(GL_POLYGON_OFFSET_POINT);
            break;
        case GPUPipelineCapability::CAPABILITY_DEPTH_READ_ONLY:
            glDepthMask(GL_FALSE);
            break;
        case GPUPipelineCapability::CAPABILITY_STENCIL:
            glEnable(GL_STENCIL_TEST);
            break;
        case GPUPipelineCapability::CAPABILITY_SCISSOR:
            glEnable(GL_SCISSOR_TEST);
            break;

        case GPUPipelineCapability::CAPABILITY_PRIMITIVE_RESTART:
            if (GLAD_GL_ARB_ES3_compatibility) {
                glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
            } else if (glPrimitiveRestartIndex != nullptr) {
                glEnable(GL_PRIMITIVE_RESTART);
                glPrimitiveRestartIndex(0xFFFFFFFF);
            }

            break;
        case GPUPipelineCapability::CAPABILITY_RASTERIZE_DISCARD:
            glEnable(GL_RASTERIZER_DISCARD);
            break;
        case GPUPipelineCapability::CAPABILITY_MULTISAMPLE:
            glEnable(GL_MULTISAMPLE);
            break;
    }

    // Enable Flag to State
    state->capabilities |= flag;
}

void GLState::disableCapability(GPUPipelineCapability cap) {
    GLPipelineState *state = &m_pipeline_state;
    // Avoid Already Disabled
    unsigned int flag = 1 << static_cast<unsigned int>(cap);
    if ((flag & state->capabilities) == 0)
        return;

    switch (cap) {
        case GPUPipelineCapability::CAPABILITY_BLENDING:
            glDisable(GL_BLEND);
            break;
        case GPUPipelineCapability::CAPABILITY_CULLING:
            glDisable(GL_CULL_FACE);
            break;

        case GPUPipelineCapability::CAPABILITY_DEPTH:
            glDisable(GL_DEPTH_TEST);
            break;
        case GPUPipelineCapability::CAPABILITY_DEPTH_OFFSET:
            glDisable(GL_POLYGON_OFFSET_FILL);
            glDisable(GL_POLYGON_OFFSET_LINE);
            glDisable(GL_POLYGON_OFFSET_POINT);
            break;
        case GPUPipelineCapability::CAPABILITY_DEPTH_READ_ONLY:
            glDepthMask(GL_TRUE);
            break;
        case GPUPipelineCapability::CAPABILITY_STENCIL:
            glDisable(GL_STENCIL_TEST);
            break;
        case GPUPipelineCapability::CAPABILITY_SCISSOR:
            glDisable(GL_SCISSOR_TEST);
            break;

        case GPUPipelineCapability::CAPABILITY_PRIMITIVE_RESTART:
            if (GLAD_GL_ARB_ES3_compatibility)
                glDisable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
            else if (glPrimitiveRestartIndex != nullptr)
                glDisable(GL_PRIMITIVE_RESTART);

            break;
        case GPUPipelineCapability::CAPABILITY_RASTERIZE_DISCARD:
            glDisable(GL_RASTERIZER_DISCARD);
            break;
        case GPUPipelineCapability::CAPABILITY_MULTISAMPLE:
            glDisable(GL_MULTISAMPLE);
            break;
    }

    // Disable Flag to State
    state->capabilities &= ~flag;
}

// ------------------------------
// OpenGL State: Pipeline Effects
// ------------------------------

void GLState::markPipelineEffect(GLPipelineEffect effect) {
    m_pipeline_effects |= 1 << static_cast<unsigned int>(effect);
}

void GLState::unmarkPipelineEffect(GLPipelineEffect effect) {
    m_pipeline_effects &= ~(1 << static_cast<unsigned int>(effect));
}

void GLState::checkPipelineEffects(GLPipelineState &check) {
    GLPipelineState *state = &m_pipeline_state;

    if (state->capabilities != check.capabilities)
        markPipelineEffect(GLPipelineEffect::PIPELINE_EFFECT_CAPABILITIES);
    if (state->program != check.program)
        markPipelineEffect(GLPipelineEffect::PIPELINE_EFFECT_PROGRAM);
    if (memcmp(&state->blending, &check.blending, sizeof(GPUPipelineBlending)) != 0)
        markPipelineEffect(GLPipelineEffect::PIPELINE_EFFECT_BLENDING);
    if (memcmp(&state->culling, &check.culling, sizeof(GPUPipelineFace)) != 0)
        markPipelineEffect(GLPipelineEffect::PIPELINE_EFFECT_CULLING);
    if (memcmp(&state->depth, &check.depth, sizeof(GPUPipelineDepth)) != 0)
        markPipelineEffect(GLPipelineEffect::PIPELINE_EFFECT_DEPTH);
    if (memcmp(&state->stencil, &check.stencil, sizeof(GPUPipelineStencil)) != 0)
        markPipelineEffect(GLPipelineEffect::PIPELINE_EFFECT_STENCIL);

    // OpenGL Viewport State
    if (state->clear_depth != check.clear_depth)
        markPipelineEffect(GLPipelineEffect::PIPELINE_EFFECT_CLEAR_DEPTH);
    if (state->clear_stencil != check.clear_stencil)
        markPipelineEffect(GLPipelineEffect::PIPELINE_EFFECT_CLEAR_STENCIL);
    if (memcmp(&state->clear_color, &check.clear_color, sizeof(GPUColor)) != 0)
        markPipelineEffect(GLPipelineEffect::PIPELINE_EFFECT_CLEAR_COLOR);
    if (memcmp(&state->color_mask, &check.color_mask, sizeof(GPUColorMask)) != 0)
        markPipelineEffect(GLPipelineEffect::PIPELINE_EFFECT_COLOR_MASK);
    if (memcmp(&state->viewport, &check.viewport, sizeof(GPURectangle)) != 0)
        markPipelineEffect(GLPipelineEffect::PIPELINE_EFFECT_VIEWPORT);
    if (memcmp(&state->scissor, &check.scissor, sizeof(GPURectangle)) != 0)
        markPipelineEffect(GLPipelineEffect::PIPELINE_EFFECT_SCISSOR);
    if (state->line_width != check.line_width)
        markPipelineEffect(GLPipelineEffect::PIPELINE_EFFECT_LINE_WIDTH);
}

// ------------------------------
// OpenGL State: Commands Effects
// ------------------------------

void GLState::markCommandsEffect(GLCommandsEffect effect) {
    m_commands_effects |= 1 << static_cast<unsigned int>(effect);
}

void GLState::unmarkCommandsEffect(GLCommandsEffect effect) {
    m_commands_effects &= ~(1 << static_cast<unsigned int>(effect));
}

void GLState::checkCommandsEffects(GLCommandsState &check) {

}
