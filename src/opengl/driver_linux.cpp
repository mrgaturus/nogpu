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

static void egl_configure_msaa(int msaa_samples, bool rgba) {
    if (msaa_samples < 0) msaa_samples = 0;
    if (msaa_samples > 16) msaa_samples = 16;

   // Configure RGBA Surface
    if (rgba) {
        attr_egl[13] = 8;
        attr_egl[15] = 32;
    } else {
        attr_egl[13] = 0;
        attr_egl[15] = 24;
    }

    // Configure Multisample Rendering
    attr_egl[17] = !! msaa_samples;
    attr_egl[19] = next_power_of_two(msaa_samples);
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

    egl_configure_msaa(msaa_samples, rgba);
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
        result &= eglTerminate(egl->display);
        free(egl); egl = next;
    }

    // Unload Native Modules
    if (driver->so_wayland) dlclose(driver->so_wayland);
    if (driver->so_x11) dlclose(driver->so_x11);
    driver->so_wayland = nullptr;
    driver->so_x11 = nullptr;

    // Clear Driver Attributes
    driver->list = nullptr;
    driver->current = nullptr;
    driver->surface = nullptr;
    // Reset Private
    m_features = 0;
    m_msaa_samples = 0;
    m_rgba = false;

    // Return Driver Shutdown Status
    if (result) GPULogger::success("[opengl] terminated EGL display");
    else GPULogger::error("[opengl] failed terminating EGL & OpenGL");
    return result;
}

// ------------------------------
// Linux OpenGL Context: Features
// ------------------------------

bool GLDriver::impl__checkInitialized() {
    return m_features > 0;
}

bool GLDriver::impl__checkRGBASurface() {
    return m_rgba;
}

bool GLDriver::impl__checkFeature(GPUDriverFeature feature) {
    return m_features & feature__flag(feature);
}

int GLDriver::impl__getMultisamplesCount() {
    return m_msaa_samples;
}

GPUDriverOption GLDriver::impl__getDriverOption() {
    return GPUDriverOption::DRIVER_OPENGL;
}

// --------------------------------------
// Linux OpenGL Context: Wayland Specific
// --------------------------------------

typedef struct wl_egl_window wl_egl_window;
typedef wl_egl_window* (*wl_egl_window_create_t) (void* surface, int w, int h);
typedef void (*wl_egl_window_resize_t) (wl_egl_window* win, int w, int h, int dx, int dy);
typedef void (*wl_egl_window_get_attached_size_t) (wl_egl_window* win, int *w, int *h);
typedef void (*wl_egl_window_destroy_t) (wl_egl_window* win);

static void configureWaylandSurface(LinuxEGLContext* gtx, void* window) {
    LinuxEGLDriver* driver = gtx->driver;
    LinuxEGL* egl = gtx->egl;

    // Open Wayland Module
    if (!driver->so_wayland) driver->so_wayland =
        dlopen("libwayland-egl.so.1", RTLD_LAZY | RTLD_GLOBAL);
    if (!driver->so_wayland) {
        GPULogger::error("[opengl] failed load libwayland-egl.so.1");
        return;
    }

    // Create Wayland EGL Window Surface
    wl_egl_window_create_t wl_egl_window_create = (wl_egl_window_create_t)
        dlsym(driver->so_wayland, "wl_egl_window_create");
    gtx->wl_surface = (void*) wl_egl_window_create(window, 64, 64);
    if (!gtx->wl_surface) {
        GPULogger::error("[opengl] failed creating wayland EGL window");
        return;
    }

    // Cache Wayland EGL Proc Symbols
    gtx->wl_resize_proc = dlsym(driver->so_wayland, "wl_egl_window_resize");
    gtx->wl_dimensions_proc = dlsym(driver->so_wayland, "wl_egl_window_get_attached_size");
    gtx->wl_destroy_proc = dlsym(driver->so_wayland, "wl_egl_window_destroy");

}

static void resizeWaylandSurface(LinuxEGLContext* gtx, int w, int h) {
    wl_egl_window_resize_t wl_egl_window_resize = (wl_egl_window_resize_t) gtx->wl_resize_proc;
    wl_egl_window_resize((wl_egl_window*) gtx->wl_surface, w, h, 0, 0);
}

// ----------------------------------
// Linux OpenGL Context: X11 Specific
// ----------------------------------

typedef struct {
	int x, y;
	int width, height;
	int egl_depth;
	int depth;
    // Unused X11 Attributes
    char unused[232];
} x11_window_attributes_t;

typedef void (*x11_XGetWindowAttributes_t)(
    void* display,
    unsigned long window,
    x11_window_attributes_t* result
);

typedef void (*x11_XResizeWindow_t)(
    void* display,
    unsigned long window,
    unsigned int w,
    unsigned int h
);

static void configureX11Surface(LinuxEGLContext* gtx, void* window) {
    LinuxEGLDriver* driver = gtx->driver;
    LinuxEGL* egl = gtx->egl;

    // Open X11 Module
    if (!driver->so_x11) driver->so_x11 =
        dlopen("libX11.so.6", RTLD_LAZY | RTLD_GLOBAL);
    if (!driver->so_x11) {
        GPULogger::error("[opengl] failed load libX11.so.6");
        return;
    }

    x11_window_attributes_t attribs;
    x11_XGetWindowAttributes_t XGetWindowAttributes =
        (x11_XGetWindowAttributes_t) dlsym(driver->so_x11, "XGetWindowAttributes");
    // Get X11 & EGL Relevant Attributes
    XGetWindowAttributes(egl->linux_display, (unsigned long) window, &attribs);
    eglGetConfigAttrib(egl->display, egl->config, EGL_BUFFER_SIZE, &attribs.egl_depth);

    // Check if Window is Transparent
    // WEIRD BEHAVIOUR: x11 window depth forces EGL depth
    //                  so EGL depth is quite useless for X11
    if (attribs.depth != attribs.egl_depth)
        GPULogger::warning("[opengl] color depth mismatch, EGL: %d ~ X11 window: %d",
            attribs.egl_depth, attribs.depth);
    gtx->linux_is_rgba = attribs.depth == 32;
}

// -----------------------------
// Linux OpenGL Context: Current
// -----------------------------

GPUContext* GLDriver::makeLinuxContext(LinuxEGLContext gtx) {
    GPUContext* ctx = nullptr;

    // Create GLContext
    if (gtx.egl && gtx.surface) {
        GLContext* ctx0 = new GLContext();
        ctx0->m_driver = this;
        ctx0->m_gtx = gtx;
        // Return GPUContext
        ctx = (GPUContext*) ctx0;
        this->cached__add(ctx);
    }

    // Return Created Context
    return ctx;
}

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
    LinuxEGLContext* gtx = &ctx->m_gtx;

    if (driver->current == gtx->egl && driver->surface == gtx->surface) {
        eglMakeCurrent(gtx->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        // Cache Current EGL Connection
        driver->current = nullptr;
        driver->surface = nullptr;
    }

    // Destroy Context Surface
    this->cached__remove(ctx);
    if (eglDestroySurface(gtx->display, gtx->surface) == EGL_FALSE)
        GPULogger::error("[opengl] failed destroying EGL surface");

    // Destroy Wayland Context Surface
    if (gtx->wl_surface) {
        wl_egl_window_destroy_t wl_egl_window_destroy =
            (wl_egl_window_destroy_t) gtx->wl_destroy_proc;
        wl_egl_window_destroy((wl_egl_window*) gtx->wl_surface);
    }
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

static LinuxEGL* createLinuxEGL(void* display, LinuxEGLOption option) {
    LinuxEGL egl = {
        .linux_display = display,
        .linux_is_x11 = option == LinuxEGLOption::LINUX_X11
    };

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

// ---------------------------------
// Linux OpenGL Context: EGL Surface
// ---------------------------------

static void configureEGLSurface(LinuxEGLContext* gtx, void* window, LinuxEGLOption option) {
    LinuxEGL* egl = gtx->egl;
    EGLSurface surface = nullptr;

    if (egl != nullptr) {
        switch (option) {
            case LinuxEGLOption::LINUX_WAYLAND:
                configureWaylandSurface(gtx, window);
                window = gtx->wl_surface;
                break;
            case LinuxEGLOption::LINUX_X11:
                configureX11Surface(gtx, window); break;
        }

        // Create EGL Surface
        surface = eglCreateWindowSurface(
            egl->display, egl->config,
            (EGLNativeWindowType) window,
            attr_surface);
    }

    // Check if the Surface was Created
    if (surface != EGL_NO_SURFACE) {
        gtx->surface = surface;
        gtx->linux_is_x11 = egl->linux_is_x11;
    } else GPULogger::error("[opengl] failed create EGL surface for %p", window);
}

static void configureEGLContext(LinuxEGLContext* gtx, void* display, LinuxEGLOption option) {
    LinuxEGL* list = gtx->driver->list;
    LinuxEGL* egl_found = list;
    // Find Already Created EGL
    while (egl_found) {
        if (egl_found->linux_display == display)
            goto FOUND_EGL;
        // Next EGL Context
        egl_found = egl_found->next;
    }

    // Create Linux EGL Display
    egl_found = createLinuxEGL(display, option);
    if (egl_found) {
        egl_found->next = list;
        egl_found->prev = nullptr;
        gtx->driver->list = egl_found;
    }

    FOUND_EGL: // Return Created Display
        gtx->egl = egl_found;
        gtx->display = (egl_found) ?
            egl_found->display : nullptr;
}

// -----------------------------------
// Linux OpenGL Context: Raw Windowing
// -----------------------------------

GPUContext* GLDriver::impl__createContext(GPUWindowX11 win) {
    void* window = (void*) win.window;
    LinuxEGLContext gtx = {.driver = &m_egl};
    configureEGLContext(&gtx, (void*) win.display, LinuxEGLOption::LINUX_X11);
    configureEGLSurface(&gtx, window, LinuxEGLOption::LINUX_X11);

    // Resize X11 Window if Success
    if (gtx.egl && gtx.surface) {
        GPULogger::success("[opengl] EGL X11 surface created for XID:%ld", win.window);
        x11_XResizeWindow_t XResizeWindow = (x11_XResizeWindow_t)
            dlsym(m_egl.so_x11, "XResizeWindow");
        XResizeWindow(gtx.egl->linux_display, win.window, win.w, win.h);
    }

    // Return Created Context
    return makeLinuxContext(gtx);
}

GPUContext* GLDriver::impl__createContext(GPUWindowWayland win) {
    LinuxEGLContext gtx = {.driver = &m_egl};
    configureEGLContext(&gtx, win.display, LinuxEGLOption::LINUX_WAYLAND);
    configureEGLSurface(&gtx, win.surface, LinuxEGLOption::LINUX_WAYLAND);

    // Resize Wayland EGL if Success
    if (gtx.egl && gtx.surface) {
        GPULogger::success("[opengl] EGL Wayland surface created for wl_surface:%p", win.surface);
        resizeWaylandSurface(&gtx, win.w, win.h);
    }

    // Return Created Context
    return makeLinuxContext(gtx);
}

// -----------------------------------
// Linux OpenGL Context: SDL Windowing
// -----------------------------------

#if defined(NOGPU_SDL3)

GPUContext* GLDriver::impl__createContext(SDL_Window *win) {
    SDL_PropertiesID win_props;
    LinuxEGLContext gtx = {.driver = &m_egl};

    // Check Valid SDL3 Window
    if (!win || (win_props = SDL_GetWindowProperties(win)) == 0) {
        GPULogger::error("invalid SDL3 window");
        return nullptr;
    }

    // Check SDL3 Window Flags
    if (SDL_GetWindowFlags(win) & (SDL_WINDOW_OPENGL | SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) {
        GPULogger::error("SDL3 window flags must not have SDL_WINDOW_OPENGL | SDL_WINDOW_VULKAN | SDL_WINDOW_METAL");
        return nullptr;
    } else if (SDL_WindowHasSurface(win) || SDL_GetRenderer(win)) {
        GPULogger::error("SDL3 window must not have a SDL_Surface or SDL_Renderer");
        return nullptr;
    }

    // Try Wayland Surface
    void* display = SDL_GetPointerProperty(win_props,
        SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
    if (display != nullptr) {
        if (SDL_HasProperty(win_props, SDL_PROP_WINDOW_WAYLAND_EGL_WINDOW_POINTER)) {
            GPULogger::error("SDL3 window must not have an egl_window");
            return nullptr;
        }

        void* surface = SDL_GetPointerProperty(win_props,
            SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
        // Create Wayland EGL Context for SDL3 Window
        configureEGLContext(&gtx, display, LinuxEGLOption::LINUX_WAYLAND);
        configureEGLSurface(&gtx, surface, LinuxEGLOption::LINUX_WAYLAND);
        if (gtx.egl && gtx.surface) {
            GPULogger::success("[opengl] EGL Wayland surface created for SDL3:%p", win);
            // Resize Wayland Surface
            int w, h; SDL_GetWindowSize(win, &w, &h);
            resizeWaylandSurface(&gtx, w, h);
            return makeLinuxContext(gtx);
        }
    }

    // Try X11 Surface
    display = SDL_GetPointerProperty(win_props,
        SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
    if (display != nullptr) {
        unsigned long xid = SDL_GetNumberProperty(win_props,
            SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);

        // Create X11 EGL Context for SDL3 Window
        configureEGLContext(&gtx, (void*) display, LinuxEGLOption::LINUX_X11);
        configureEGLSurface(&gtx, (void*) xid, LinuxEGLOption::LINUX_X11);
        if (gtx.egl && gtx.surface) {
            GPULogger::success("[opengl] EGL X11 surface created for SDL3:%p", win);
            return makeLinuxContext(gtx);
        }
    }

    // No Valid SDL3 Window Found
    GPULogger::error("SDL3 window is not Wayland or X11");
    return nullptr;
}

#elif defined(NOGPU_SDL2)
#include <SDL2/SDL_syswm.h>

GPUContext* GLDriver::impl__createContext(SDL_Window *win) {
    SDL_SysWMinfo syswm;
    // Get SDL2 Native Info
    if (!win || !SDL_GetWindowWMInfo(win, &syswm)) {
        GPULogger::error("invalid SDL2 window");
        return nullptr;
    }

    // Check SDL2 Window Flags
    if (SDL_GetWindowFlags(win) & (SDL_WINDOW_OPENGL | SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) {
        GPULogger::error("SDL2 window flags must not have SDL_WINDOW_OPENGL | SDL_WINDOW_VULKAN | SDL_WINDOW_METAL");
        return nullptr;
    } else if (SDL_HasWindowSurface(win) || SDL_GetRenderer(win)) {
        GPULogger::error("SDL2 window must not have a SDL_Surface or SDL_Renderer");
        return nullptr;
    }

    // Create EGL Context
    LinuxEGLContext gtx = {.driver = &m_egl};
    switch (syswm.subsystem) {
        case SDL_SYSWM_WAYLAND:
            if (syswm.info.wl.egl_window) {
                GPULogger::error("SDL2 window must not have an egl_window");
                return nullptr;
            }

            configureEGLContext(&gtx, syswm.info.wl.display, LinuxEGLOption::LINUX_WAYLAND);
            configureEGLSurface(&gtx, syswm.info.wl.surface, LinuxEGLOption::LINUX_WAYLAND);
            if (gtx.egl && gtx.surface) {
                GPULogger::success("[opengl] EGL Wayland surface created for SDL2:%p", win);
                // Resize Wayland Surface
                int w, h; SDL_GetWindowSize(win, &w, &h);
                resizeWaylandSurface(&gtx, w, h);
            } break;
        case SDL_SYSWM_X11:
            configureEGLContext(&gtx, (void*) syswm.info.x11.display, LinuxEGLOption::LINUX_X11);
            configureEGLSurface(&gtx, (void*) syswm.info.x11.window, LinuxEGLOption::LINUX_X11);
            if (gtx.egl && gtx.surface) {
                GPULogger::success("[opengl] EGL X11 surface created for SDL2:%p", win);
            } break;
        default: // Invalid Windowing
            GPULogger::error("SDL2 window is not Wayland or X11");
            return nullptr;
    }

    // Return Created Context
    return makeLinuxContext(gtx);
}

#endif
