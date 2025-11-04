// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/commands.h"
#include "private/context.h"
#include "private/glad.h"

GLFence::GLFence(GLContext* ctx) {
    ctx->makeCurrent(this);

    // Stamp Fence Sync
    m_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    m_ctx = ctx;
}

void GLFence::destroy() {
    m_ctx->makeCurrent(this);

    // Destroy Fence Sync
    if (m_sync) glDeleteSync(m_sync);
    delete this;
}

// -----------------------
// GLFence: Syncronization
// -----------------------

void GLFence::waitCPU() {
    m_ctx->makeCurrent(this);
    if (m_sync == nullptr)
        return;

	GLbitfield flags = 0;
	GLuint64 duration = 0;
    // Stall CPU until Fence Signaled
	while (true) {
		GLenum status = glClientWaitSync(m_sync, flags, duration);
		if (status == GL_ALREADY_SIGNALED || status == GL_CONDITION_SATISFIED)
			break;
		if (status == GL_WAIT_FAILED) {
            GPUReport::warning("failed fence sync");
			break;
        }

        // 1 second in nanoseconds.
		flags = GL_SYNC_FLUSH_COMMANDS_BIT;
		duration = 1000000000;
	}
}

void GLFence::waitGPU() {
    m_ctx->makeCurrent(this);

    // Stall GL Queue until Fence Signaled
    if (m_sync != nullptr)
        glWaitSync(m_sync, 0, GL_TIMEOUT_IGNORED);
}

bool GLFence::completed() {
    m_ctx->makeCurrent(this);
    if (m_sync == nullptr)
        return true;

	GLenum status = glClientWaitSync(m_sync, 0, 0);
	if (status == GL_ALREADY_SIGNALED || status == GL_CONDITION_SATISFIED)
		return true;
	if (status == GL_WAIT_FAILED) {
        GPUReport::warning("failed fence sync");
		return true;
    }

    // Not Completed Yet
	return false;
}


// -----------------
// GLFence: Creation
// -----------------

GPUFence* GLCommands::syncFence() {
    m_ctx->makeCurrent(this);
    return new GLFence(m_ctx);
}

GPUFence* GLContext::syncFence() {
    return new GLFence(this);
}
