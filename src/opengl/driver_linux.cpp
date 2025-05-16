// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include <cstdlib>
// Include OpenGL
#include "glad/glad.h"
#include "opengl.h"

static EGLint attr_egl[] = {
    EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
    // EGL Color Channels
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
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

GLDriver::GLDriver(int msaa_samples, bool rgba) {
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
        if (GLAD_GL_ARB_debug_output) // Debug Feature
            features |= feature__flag(GPUDriverFeature::DRIVER_FEATURE_DEBUG);
        if (GLAD_GL_ARB_compute_shader) // Compute Shader Feature
            features |= feature__flag(GPUDriverFeature::DRIVER_FEATURE_COMPUTE);
        if (GLAD_GL_ARB_shader_image_load_store) // Memory Barrier Feature
            features |= feature__flag(GPUDriverFeature::DRIVER_FEATURE_BARRIER);
        if (GLAD_GL_ARB_gl_spirv) { // SPIR-V Shader Feature
            features |= feature__flag(GPUDriverFeature::DRIVER_SHADER_SPIRV);
            features |= feature__flag(GPUDriverFeature::DRIVER_SHADER_LOW_SPIRV);
        }

        // Define Features
        m_features = features;
        m_msaa_samples = msaa_samples;
        m_rgba = rgba;
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
    LinuxEGL *egl = m_egl_list;
    while (egl) {
        LinuxEGL* next = egl->next;
        result &= eglTerminate(egl);
        free(egl); egl = next;
    }

    // Clear Driver Attributes
    m_egl_list = nullptr;
    m_egl_current = nullptr;
    m_egl_surface = nullptr;
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

bool GLDriver::impl__getTransparency() {
    return m_rgba;
}

// -----------------------------
// Linux OpenGL Context: Current
// -----------------------------

void GLDriver::makeCurrent(GLContext* ctx) {
    if (m_egl_current != ctx->m_egl || m_egl_surface != ctx->m_egl_surface) {
        EGLSurface egl_surface = ctx->m_egl_surface;
        EGLContext egl_ctx = ctx->m_egl->context;

        // Change Current EGL Connection
        eglMakeCurrent(ctx->m_egl->display,
            egl_surface, egl_surface, egl_ctx);
        m_egl_current = ctx->m_egl;
        m_egl_surface = egl_surface;
    }
}

void GLDriver::makeDestroyed(GLContext* ctx) {
    if (m_egl_current == ctx->m_egl && m_egl_surface == ctx->m_egl_surface) {
        eglMakeCurrent(ctx->m_egl->display,
            EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        // Cache Current EGL Connection
        m_egl_current = nullptr;
        m_egl_surface = nullptr;
    }

    // Destroy Context Surface
    this->cached__remove(ctx);
    if (eglDestroySurface(ctx->m_egl->display, ctx->m_egl_surface) == EGL_FALSE)
        GPULogger::error("[opengl] failed destroying EGL surface");
}

// ---------------------------
// Linux OpenGL Context: Debug
// ---------------------------

#ifdef NOGPU_DEBUG
void GLAD_API_PTR nogpu_debug_callback(
    GLenum source, 
    GLenum type, 
    unsigned int id, 
    GLenum severity, 
    GLsizei length, 
    const char *message, 
    const void *user)
{
    int level = 0;
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH_ARB: level = 3; break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB: level = 2; break;
        case GL_DEBUG_SEVERITY_LOW_ARB: level = 1; break;
    }

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR_ARB:
            GPULogger::error("[opengl:debug %d %d - %p] %s",
                id, level, user, message);
            GPULogger::stacktrace();
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:
            GPULogger::warning("[opengl:debug %d %d - %p] %s",
                id, level, user, message);
            GPULogger::stacktrace();
            break;
        case GL_DEBUG_TYPE_PORTABILITY_ARB:
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:
        case GL_DEBUG_TYPE_OTHER_ARB:
            GPULogger::info("[opengl:debug %d %d - %p] %s",
                id, level, user, message);
            break;
        default: break;
    }
}
#endif

// ---------------------------------
// Linux OpenGL Context: EGL Display
// ---------------------------------

enum class LinuxEGLOption : EGLenum {
    LINUX_WAYLAND = 0x31D8,
    LINUX_X11 = 0x31D5
};

static LinuxEGL* createLinuxEGL(void* display, LinuxEGLOption option, int msaa_samples) {
    LinuxEGL egl = {.native = display};
    EGLenum egl_option = (EGLenum) option;
    int egl_num_config;
    int egl_major, egl_minor;

    // Create Native Specific EGL Display
    egl.display = eglGetPlatformDisplay(egl_option, display, nullptr);
    if (egl.display == EGL_NO_DISPLAY) {
        GPULogger::error("[opengl] failed create EGL display");
        return nullptr;
    }

    // Initialize EGL Display
    if (eglInitialize(egl.display, &egl_major, &egl_minor) == EGL_FALSE) {
        GPULogger::error("[opengl] failed initialize EGL for %x", egl_option);
        goto TERMINATE_EGL;
    }

    // Configure EGL Display
    attr_egl[15] = 1;
    attr_egl[17] = next_power_of_two(msaa_samples);
    if (eglChooseConfig(egl.display, attr_egl, &egl.config, 1, &egl_num_config) == EGL_FALSE) {
        GPULogger::error("[opengl] failed configure EGL");
        goto TERMINATE_EGL;
    }

    #ifdef NOGPU_DEBUG
        if (GLAD_GL_ARB_debug_output) {
            attr_context[7] = EGL_TRUE;
        }
    #endif

    // Create EGL Native Context
    egl.context = eglCreateContext(egl.display, egl.config, EGL_NO_CONTEXT, attr_context);
    if (egl.context == EGL_NO_CONTEXT) {
        GPULogger::error("[opengl] device doesn't support OpenGL 3.3");
        goto TERMINATE_EGL;
    }

    #ifdef NOGPU_DEBUG
        if (GLAD_GL_ARB_debug_output) {
            eglMakeCurrent(egl.display,
                EGL_NO_SURFACE, EGL_NO_SURFACE, egl.context);

            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB); 
            glDebugMessageCallbackARB(nogpu_debug_callback, egl.display);
            glDebugMessageControlARB( // Enable All Messages
                GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
                0, nullptr, GL_TRUE);

            eglMakeCurrent(egl.display,
                EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        }
    #endif

    { // Return Created EGL
        LinuxEGL* egl_display = (LinuxEGL*)
            malloc(sizeof(LinuxEGL));
        *egl_display = egl;
        egl_display->next = nullptr;
        return egl_display;
    }

    // Terminate Display
TERMINATE_EGL:
    eglTerminate(egl.display);
    return nullptr;
}

// ---------------------------------
// Linux OpenGL Context: EGL Surface
// ---------------------------------

static LinuxEGL* getLinuxEGL(LinuxEGL** egl, void* display, LinuxEGLOption option, int msaa_samples) {
    LinuxEGL* egl_found = *egl;
    // Find Already Created EGL
    while (egl_found) {
        if (egl_found->native == display)
            return egl_found;
        egl_found = egl_found->next;
    }

    // Create Linux EGL Display
    egl_found = createLinuxEGL(display, option, msaa_samples);
    if (egl_found) {
        egl_found->next = *egl;
        egl_found->prev = nullptr;
        *egl = egl_found;
    }

    // Return Created Display
    return egl_found;
}

static EGLSurface createLinuxEGLSurface(LinuxEGL* egl, void* native, bool rgba) {
    attr_surface[3] = (rgba) ? EGL_TEXTURE_RGBA : EGL_TEXTURE_RGB;
    EGLSurface surface = eglCreateWindowSurface(
        egl->display, egl->config,
        (EGLNativeWindowType) native,
        attr_surface);

    // Check if the Surface was Created
    if (surface == EGL_NO_SURFACE) {
        GPULogger::error("[opengl] failed create EGL surface for %p", native);
        return surface;
    }

    // Return Created
    return surface;
}

// -----------------------------------
// Linux OpenGL Context: SDL Windowing
// -----------------------------------

#if defined(NOGPU_SDL3)

GPUContext* impl__createContext(SDL_Window *win) {
    return nullptr;
}

#elif defined(NOGPU_SDL2)
#include <SDL2/SDL_syswm.h>

GPUContext* GLDriver::impl__createContext(SDL_Window *win) {
    SDL_SysWMinfo syswm;
    GPUContext* ctx = nullptr;
    // Get SDL2 Native Info
    if (!win || !SDL_GetWindowWMInfo(win, &syswm)) {
        GPULogger::error("invalid SDL2 window");
        return ctx;
    }

    // Check Window Flags
    if (SDL_GetWindowFlags(win) & (SDL_WINDOW_OPENGL | SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) {
        GPULogger::error("SDL2 window flags must not have SDL_WINDOW_OPENGL | SDL_WINDOW_VULKAN | SDL_WINDOW_METAL");
        return ctx;
    } else if (SDL_HasWindowSurface(win) || SDL_GetRenderer(win)) {
        GPULogger::error("SDL2 window must not have a SDL_Surface or SDL_Renderer");
        return ctx;
    }

    // Create EGL Context
    LinuxEGL* egl = nullptr;
    EGLSurface surface = nullptr;
    switch (syswm.subsystem) {
        case SDL_SYSWM_WAYLAND:
            egl = getLinuxEGL(&m_egl_list, syswm.info.wl.display,
                LinuxEGLOption::LINUX_WAYLAND, m_msaa_samples);
            // Create EGL Surface
            if (egl) {
                GPULogger::success("[opengl] EGL Wayland context created");
                surface = createLinuxEGLSurface(egl,
                    syswm.info.wl.surface, m_rgba);
            } break;
        case SDL_SYSWM_X11:
            egl = getLinuxEGL(&m_egl_list, syswm.info.x11.display,
                LinuxEGLOption::LINUX_X11, m_msaa_samples);
            // Create EGL Surface
            if (egl) {
                GPULogger::success("[opengl] EGL X11 context created");
                surface = createLinuxEGLSurface(egl,
                    (void*) syswm.info.x11.window, m_rgba);
            } break;
        default: // Invalid Windowing
            GPULogger::error("SDL2 window is not Wayland or X11");
            return nullptr;
    }

    // Create GLContext
    if (egl && surface) {
        GLContext* gl_ctx = new GLContext();
        gl_ctx->m_driver = this;
        gl_ctx->m_window = win;
        gl_ctx->m_egl = egl;
        gl_ctx->m_egl_surface = surface;
        // Return GPUContext
        ctx = (GPUContext*) gl_ctx;
        this->cached__add(ctx);
    }

    // Return Created Context
    return ctx;
}

#endif
