// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "glad/glad.h"
#include "opengl.h"

static EGLint attr_egl[] = {
    EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
    // EGL Color Channels
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    // EGL Special Buffers
    EGL_DEPTH_SIZE, 24,
    EGL_STENCIL_SIZE, 8,
    // Optional MSAA
    EGL_SAMPLES, 0,
    EGL_SAMPLE_BUFFERS, 0,
    EGL_NONE,
};

static EGLint attr_context[] = {
    EGL_CONTEXT_MAJOR_VERSION, 3,
    EGL_CONTEXT_MINOR_VERSION, 3,
    EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
    EGL_CONTEXT_OPENGL_DEBUG, EGL_FALSE,
    EGL_NONE
};

static EGLint attr_surface[] = {
    EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
    EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGB,
    EGL_NONE
};

// --------------------------------
// Linux OpenGL Driver: Constructor
// --------------------------------

GLDriver::GLDriver(int msaa_samples) {
    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;
    int egl_num_config;
    int egl_major, egl_minor;

    // Use OpenGL API for EGL
    if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE) {
        GPULogger::error("[opengl] failed initialize OpenGL API");
        return;
    }

    // Create Dummy Display to Check OpenGL
    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (egl_display == EGL_NO_DISPLAY) {
        GPULogger::error("[opengl] failed create EGL display");
        return;
    }

    // Initialize and Configure Display
    if (eglInitialize(egl_display, &egl_major, &egl_minor) == EGL_FALSE) {
        GPULogger::error("[opengl] failed initialize EGL"); goto TERMINATE_EGL;
    }
    if (eglChooseConfig(egl_display, attr_egl, &egl_config, 1, &egl_num_config) == EGL_FALSE) {
        GPULogger::error("[opengl] failed configure EGL"); goto TERMINATE_EGL;
    }

    // Create Dummy Context
    egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, attr_context);
    if (egl_context == EGL_NO_CONTEXT) {
        GPULogger::error("[opengl] device doesn't support OpenGL 3.3"); goto TERMINATE_EGL;
    }
    if (eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, egl_context) == EGL_FALSE) {
        GPULogger::error("[opengl] failed check OpenGL features"); goto TERMINATE_EGL;
    }

    // Load OpenGL Functions
    if (gladLoadGL((GLADloadfunc) eglGetProcAddress) != 0) {
        if (GLAD_GL_VERSION_3_3 == 0) {
            GPULogger::error("[opengl] device doesn't support OpenGL 3.3");
            goto TERMINATE_EGL;
        }

        unsigned int features =
            feature__flag(GPUDriverFeature::DRIVER_FEATURE_RASTERIZE) |
            feature__flag(GPUDriverFeature::DRIVER_SHADER_GLSL) |
            feature__flag(GPUDriverFeature::DRIVER_SHADER_LOW_GLSL);
        // Check Extra Extensions
        if (GLAD_GL_ARB_compute_shader) // Check Compute Shader Feature
            features |= feature__flag(GPUDriverFeature::DRIVER_FEATURE_COMPUTE);
        if (GLAD_GL_ARB_shader_image_load_store) // Check Memory Barrier Feature
            features |= feature__flag(GPUDriverFeature::DRIVER_FEATURE_BARRIER);
        if (GLAD_GL_ARB_gl_spirv) { // Check SPIR-V Shader Feature
            features |= feature__flag(GPUDriverFeature::DRIVER_SHADER_SPIRV);
            features |= feature__flag(GPUDriverFeature::DRIVER_SHADER_LOW_SPIRV);
        }

        m_features = features;
        m_msaa_samples = msaa_samples;
        // Output EGL and OpenGL Information
        const char* vendor = (const char*) glGetString(0x1F02);
        GPULogger::success("[opengl] EGL version: %d %d", egl_major, egl_minor);
        GPULogger::success("[opengl] OpenGL version %s", vendor);
    } else {
        GPULogger::error("[opengl] failed loading OpenGL functions"); goto TERMINATE_EGL;
    }

TERMINATE_EGL:
    eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglTerminate(egl_display);
}

bool GLDriver::impl__shutdown() {
    bool result = true;
    // Shutdown EGL Context
    if (m_egl_display_x11)
        result &= eglTerminate(m_egl_display_x11);
    if (m_egl_display_wayland)
        result &= eglTerminate(m_egl_display_wayland);

    // Reset EGL Context
    m_egl_config_wayland = nullptr;
    m_egl_context_wayland = nullptr;
    m_egl_display_wayland = nullptr;
    m_egl_config_x11 = nullptr;
    m_egl_context_x11 = nullptr;
    m_egl_display_x11 = nullptr;
    // Reset Private
    m_features = 0;
    m_msaa_samples = 0;

    // Return Driver Shutdown Status
    if (result) GPULogger::success("[opengl] terminated EGL & OpenGL");
    else GPULogger::error("[opengl] failed terminating EGL & OpenGL");
    return result;
}

// ------------------------------
// Linux OpenGL Context: Features
// ------------------------------

bool GLDriver::impl__checkInitialized() {
    return m_features > 0;
}

bool GLDriver::impl__checkFeature(GPUDriverFeature feature) {
    return m_features & feature__flag(feature);
}

GPUDriverOption GLDriver::impl__getDriverOption() {
    return GPUDriverOption::DRIVER_OPENGL;
}

int GLDriver::impl__getMultisamplesCount() {
    return m_msaa_samples;
}

// -------------------------
// Linux OpenGL Context: SDL
// -------------------------

#if defined(NOGPU_SDL2) || defined(NOGPU_SDL3)
GPUContext* impl__createContext(SDL_Window *win) {
    return nullptr;
}
#endif
