// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/program.h"
#include "private/state.hpp"
#include "private/pipeline.h"
#include "private/glad.h"
#include <cstring>

void GLState::resolvePipelineEffect(GLPipelineEffect effect) {
    GLPipelineState* state = &m_pipeline_state;
    GLPipelineState* current = &m_pipeline_current->m_state;

    switch (effect) {
        case GLPipelineEffect::PIPELINE_EFFECT_CAPABILITIES: {
            unsigned int caps = current->capabilities;
            unsigned int delta = state->capabilities ^ caps;

            // Resolve OpenGL Capability
            while (delta != 0) {
                unsigned int index = __builtin_ctz(delta);
                GPUPipelineCapability cap = static_cast<GPUPipelineCapability>(index);

                // Enable or Disable Capability
                if (((caps >> index) & 1) != 0)
                    enableCapability(cap);
                else disableCapability(cap);
                delta &= ~(1 << index);
            }
        } break;

        case GLPipelineEffect::PIPELINE_EFFECT_PROGRAM: {
            GLProgram *program = current->program;
            glUseProgram(program->m_program);
            state->program = program;
        } break;

        case GLPipelineEffect::PIPELINE_EFFECT_BLENDING: {
            GPUPipelineBlending *blend0 = &state->blending;
            GPUPipelineBlending *blend1 = &current->blending;

            // Check Blending Equation Changes
            if (memcmp(&blend0->equation, &blend1->equation, sizeof(blend0->equation)) != 0) {
                auto eq = &blend1->equation;
                glBlendEquationSeparate(toValue(eq->rgb), toValue(eq->alpha));
                blend0->equation = blend1->equation;
            }

            // Check Blending Factor Changes
            if (memcmp(&blend0->factor, &blend1->factor, sizeof(blend0->factor)) != 0) {
                auto factor = &blend1->factor;
                glBlendFuncSeparate(
                    toValue(factor->srcRGB),
                    toValue(factor->dstRGB),
                    toValue(factor->srcAlpha),
                    toValue(factor->srcRGB));
                blend0->factor = blend1->factor;
            }

            // Check Blending Color Changes
            if (memcmp(&blend0->color, &blend1->color, sizeof(blend0->color)) != 0) {
                auto color = &blend1->color;
                glBlendColor(color->r, color->g, color->b, color->a);
                blend0->color = blend1->color;
            }
        } break;

        case GLPipelineEffect::PIPELINE_EFFECT_CULLING: {
            GPUPipelineFace *cull0 = &state->culling;
            GPUPipelineFace *cull1 = &current->culling;

            // Change OpenGL Culling
            if (memcmp(cull0, cull1, sizeof(GPUPipelineFace)) != 0) {
                glCullFace(toValue(cull1->mode));
                glFrontFace(toValue(cull1->winding));
                state->culling = current->culling;
            }
        } break;

        case GLPipelineEffect::PIPELINE_EFFECT_DEPTH: {
            GPUPipelineDepth *depth0 = &state->depth;
            GPUPipelineDepth *depth1 = &current->depth;

            // Change OpenGL Depth
            if (memcmp(depth0, depth1, sizeof(GPUPipelineDepth)) != 0) {
                glDepthFunc(toValue(depth1->condition));
                glDepthRange(depth1->range.nearMin, depth1->range.farMax);
                glPolygonOffset(depth1->offset.factor, depth1->offset.units);
                state->depth = current->depth;
            }
        } break;

        case GLPipelineEffect::PIPELINE_EFFECT_STENCIL: {
            GPUPipelineStencil *stencil0 = &state->stencil;
            GPUPipelineStencil *stencil1 = &current->stencil;

            // Check Stencil Function Changes
            if (memcmp(&stencil0->function, &stencil1->function, sizeof(stencil0->function)) != 0) {
                auto func = &stencil1->function;
                glStencilFuncSeparate(
                    toValue(func->face),
                    toValue(func->condition),
                    func->test,
                    func->mask);
                stencil0->function = stencil1->function;
            }

            // Check Stencil Mask Changes
            if (memcmp(&stencil0->mask, &stencil1->mask, sizeof(stencil0->mask)) != 0) {
                auto mask = &stencil1->mask;
                glStencilMaskSeparate(GL_FRONT, mask->front);
                glStencilMaskSeparate(GL_BACK, mask->back);
                stencil0->mask = stencil1->mask;
            }

            // Check Stencil Mode Changes
            if (memcmp(&stencil0->mode, &stencil1->mode, sizeof(stencil0->mode)) != 0) {
                auto mode = &stencil1->mode;
                glStencilOpSeparate(
                    toValue(mode->face),
                    toValue(mode->fail),
                    toValue(mode->pass),
                    toValue(mode->pass_depth));
                stencil0->mode = stencil1->mode;
            }
        } break;

        // -- OpenGL Viewport State --
        case GLPipelineEffect::PIPELINE_EFFECT_CLEAR_DEPTH: {
            glClearDepth(current->clear_depth);
            state->clear_depth = current->clear_depth;
        } break;
        case GLPipelineEffect::PIPELINE_EFFECT_CLEAR_STENCIL: {
            glClearStencil(current->clear_stencil);
            state->clear_stencil = current->clear_stencil;
        } break;
        case GLPipelineEffect::PIPELINE_EFFECT_CLEAR_COLOR: {
            GPUColor color = current->clear_color;
            glClearColor(color.r, color.g, color.b, color.a);
            state->clear_color = color;
        } break;
        case GLPipelineEffect::PIPELINE_EFFECT_COLOR_MASK: {
            GPUColorMask mask = current->color_mask;
            glColorMask(mask.r, mask.g, mask.b, mask.a);
            state->color_mask = mask;
        } break;

        case GLPipelineEffect::PIPELINE_EFFECT_VIEWPORT: {
            GPURectangle rect = current->viewport;
            glViewport(rect.x, rect.y, rect.w, rect.h);
            state->viewport = current->viewport;
        } break;
        case GLPipelineEffect::PIPELINE_EFFECT_SCISSOR: {
            GPURectangle rect = current->scissor;
            glScissor(rect.x, rect.y, rect.w, rect.h);
            state->scissor = current->scissor;
        } break;

        case GLPipelineEffect::PIPELINE_EFFECT_LINE_WIDTH: {
            glLineWidth(current->line_width);
            state->line_width = current->line_width;
        } break;
    }
}

void GLState::resolvePipelineEffects() {
    if (m_pipeline_effects == 0)
        return;

    // Resolve Pipeline Flags
    unsigned int effects = m_pipeline_effects;
    while (effects != 0) {
        unsigned int index = __builtin_ctz(effects);
        GLPipelineEffect effect = static_cast<GLPipelineEffect>(index);
        resolvePipelineEffect(effect);
        effects &= ~(1 << index);
    }

    // Replace Pipeline Effects
    m_pipeline_effects = effects;
}
