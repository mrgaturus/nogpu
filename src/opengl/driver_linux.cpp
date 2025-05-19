// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include <cstdlib>
#include <dlfcn.h>
// Include OpenGL
#include "glad/glad.h"
#include "opengl.h"

static EGLint attr_egl[] = {
    EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
    EGL_DEPTH_SIZE, 24,
    EGL_STENCIL_SIZE, 8,
    // EGL Color Channels
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 0,
    EGL_BUFFER_SIZE, 24,
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
    EGL_NONE
};

// --------------------------------
// Linux OpenGL Driver: Constructor
// --------------------------------

static void egl_configure_attributes(int msaa_samples, bool rgba) {
    if (msaa_samples < 0) msaa_samples = 0;
    if (msaa_samples > 16) msaa_samples = 16;
    // Configure Multisample Rendering
    attr_egl[17] = !! msaa_samples;
    attr_egl[19] = next_power_of_two(msaa_samples);

    // Configure RGBA Surface
    if (rgba) {
        attr_egl[13] = 8;
        attr_egl[15] = 32;
    } else {
        attr_egl[13] = 0;
        attr_egl[15] = 24;
    }
}

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

    egl_configure_attributes(msaa_samples, rgba);
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
        GPULogger::success("[opengl] EGL version: %d.%d", egl_major, egl_minor);
        GPULogger::success("[opengl] OpenGL version: %s", vendor);
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
    LinuxEGLDriver* driver = &m_egl;
    LinuxEGL *egl = driver->list;
    while (egl) {
        LinuxEGL* next = egl->next;
        result &= eglTerminate(egl);
        free(egl); egl = next;
    }

    // Clear Driver Attributes
    driver->list = nullptr;
    driver->current = nullptr;
    driver->surface = nullptr;
    // Reset Private
    m_features = 0;
    m_msaa_samples = 0;
    m_rgba = false;

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

bool GLDriver::impl__checkRGBASurface() {
    return m_rgba;
}

int GLDriver::impl__getMultisamplesCount() {
    return m_msaa_samples;
}

GPUDriverOption GLDriver::impl__getDriverOption() {
    return GPUDriverOption::DRIVER_OPENGL;
}

// -----------------------------
// Linux OpenGL Context: Current
// -----------------------------

void GLDriver::makeCurrent(GLContext* ctx) {
    LinuxEGLDriver* driver = &m_egl;
    LinuxEGLContext gtx = ctx->m_gtx;

    if (driver->current != gtx.egl || driver->surface != gtx.surface) {
        EGLSurface egl_surface = gtx.surface;
        EGLContext egl_ctx = gtx.egl->context;

        // Change Current EGL Connection
        eglMakeCurrent(gtx.display,
            egl_surface, egl_surface, egl_ctx);

        driver->current = gtx.egl;
        driver->surface = egl_surface;
    }
}

void GLDriver::makeDestroyed(GLContext* ctx) {
    LinuxEGLDriver* driver = &m_egl;
    LinuxEGLContext gtx = ctx->m_gtx;

    if (driver->current == gtx.egl && driver->surface == gtx.surface) {
        eglMakeCurrent(gtx.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        // Cache Current EGL Connection
        driver->current = nullptr;
        driver->surface = nullptr;
    }

    // Destroy Context Surface
    this->cached__remove(ctx);
    if (eglDestroySurface(gtx.display, gtx.surface) == EGL_FALSE)
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

    // Return Created EGL
    RETURN_EGL: {
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

// ----------------------------------
// Linux OpenGL Context: X11 Specific
// ----------------------------------

typedef struct {
	int x, y;
	int width, height;
	int border_width;
	int depth;
	void *visual_raw;
    char unused[224];
} x11_window_attributes_t;

typedef void (x11_XGetWindowAttributes_t)(
    void* display,
    unsigned long window,
    x11_window_attributes_t* result
);

typedef unsigned long (x11_XVisualIDFromVisual_t)(
    void* visual_raw
);

static EGLConfig chooseX11EGLConfig(LinuxEGL* egl, unsigned long window) {
    return nullptr;
}

// ---------------------------------
// Linux OpenGL Context: EGL Surface
// ---------------------------------

static LinuxEGL* getLinuxEGL(LinuxEGLDriver* driver, void* display, LinuxEGLOption option, int msaa_samples) {
    LinuxEGL* egl_found = driver->list;
    // Find Already Created EGL
    while (egl_found) {
        if (egl_found->native == display)
            return egl_found;
        egl_found = egl_found->next;
    }

    // Create Linux EGL Display
    egl_found = createLinuxEGL(display, option, msaa_samples);
    if (egl_found) {
        egl_found->next = driver->list;
        egl_found->prev = nullptr;
        driver->list = egl_found;
    }

    // Return Created Display
    return egl_found;
}

static EGLSurface createLinuxEGLSurface(LinuxEGL* egl, void* native, bool rgba) {
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

GPUContext* GLDriver::impl__createContext(SDL_Window *win) {
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
    LinuxEGLContext gtx = {};
    switch (syswm.subsystem) {
        case SDL_SYSWM_WAYLAND:
            gtx.egl = getLinuxEGL(&m_egl, syswm.info.wl.display,
                LinuxEGLOption::LINUX_WAYLAND, m_msaa_samples);
            // Create EGL Surface
            if (gtx.egl) {
                GPULogger::success("[opengl] EGL Wayland context created for SDL2");
                gtx.surface = createLinuxEGLSurface(gtx.egl,
                    syswm.info.wl.surface, m_rgba);
            } break;
        case SDL_SYSWM_X11:
            gtx.egl = getLinuxEGL(&m_egl, syswm.info.x11.display,
                LinuxEGLOption::LINUX_X11, m_msaa_samples);
            // Create EGL Surface
            if (gtx.egl) {
                GPULogger::success("[opengl] EGL X11 context created for SDL2");
                gtx.surface = createLinuxEGLSurface(gtx.egl,
                    (void*) syswm.info.x11.window, m_rgba);
            } break;
        default: // Invalid Windowing
            GPULogger::error("SDL2 window is not Wayland or X11");
            return nullptr;
    }

    // Create GLContext
    if (gtx.egl && gtx.surface) {
        gtx.display = gtx.egl->display;
        GLContext* ctx0 = new GLContext();
        ctx0->m_driver = this;
        ctx0->m_window = win;
        ctx0->m_gtx = gtx;
        // Return GPUContext
        ctx = (GPUContext*) ctx0;
        this->cached__add(ctx);
    }

    // Return Created Context
    return ctx;
}

#endif
