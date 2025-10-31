// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_PIPELINE_H
#define OPENGL_PIPELINE_H
#include "state.hpp"

class GLContext;
class GLProgram;
class GLPipeline : GPUPipeline {
    GLPipelineState m_state;
    GLState *m_ctx_state;
    GLContext* m_ctx;

    // Pipeline Capabilites
    bool checkCapability(GPUPipelineCapability cap) override;
    void enableCapability(GPUPipelineCapability cap) override;
    void disableCapability(GPUPipelineCapability cap) override;

    // GPU Context State: Basics
    virtual void setProgram(GPUProgram *program) override;
    virtual void setBlending(GPUPipelineBlending blending) override;
    virtual void setCulling(GPUPipelineFace face) override;
    virtual void setDepth(GPUPipelineDepth depth) override;
    virtual void setStencil(GPUPipelineStencil stencil) override;
    // GPU Context State: Viewport
    virtual void setClearDepth(float depth) override;
    virtual void setClearStencil(int mask) override;
    virtual void setClearColor(GPUColor color) override;
    virtual void setColorMask(GPUColorMask mask) override;
    virtual void setViewport(GPURectangle rect) override;
    virtual void setScissor(GPURectangle rect) override;
    virtual void setLineWidth(float width) override;

    // Pipeline Attributes: Getters
    virtual GPUProgram* getProgram() override;
    virtual GPUPipelineBlending getBlending() override;
    virtual GPUPipelineFace getCulling() override;
    virtual GPUPipelineDepth getDepth() override;
    virtual GPUPipelineStencil getStencil() override;
    // GPU Context State: Viewport
    virtual float getClearDepth() override;
    virtual int getClearStencil() override;
    virtual GPUColor getClearColor() override;
    virtual GPUColorMask getColorMask() override;
    virtual GPURectangle getViewport() override;
    virtual GPURectangle getScissor() override;
    virtual float getLineWidth() override;

    protected:
        GLPipeline(GLContext* ctx);
        void destroy() override;
        friend GLContext;
};

#endif // OPENGL_PIPELINE_H