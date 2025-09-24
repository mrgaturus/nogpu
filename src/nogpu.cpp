// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <opengl/private/driver.h>
#include <nogpu_private.h>
#include <nogpu.h>

// Global Driver Definition
GPUDriver* GPUDriver::m_driver = nullptr;
bool GPUDriver::initialize(GPUDriverOption driver) {
    if (m_driver) {
        GPUReport::error("driver already initialized");
        return false;
    }

    // XXX: for now OpenGL to whole api battle test
    #if defined(__unix__)
    OPENGL_DRIVER: {
        GLDriver* gl = new GLDriver();
        if (gl) m_driver = gl;
        else delete gl;
    }
    #endif

    // Check Driver Initialized
    return !! m_driver;
}

bool GPUDriver::impl__checkDriver() {
    bool check = m_driver != nullptr;
    if (!check) GPUReport::error("driver not initialized");
    // Return Driver Initialized
    return check;
}

bool GPUDriver::shutdown() {
    bool result = (m_driver) &&
        m_driver->impl__shutdown();
    // Remove Current Driver
    m_driver = nullptr;
    return result;
}

// ------------------
// GPU Driver Methods
// ------------------

GPUDevice* GPUDriver::createDevice(GPUDeviceOption option, int samples, bool rgba) {
    GPUDevice* device = nullptr;
    if (impl__checkDriver())
        device = m_driver->impl__createDevice(option, samples, rgba);
    // Return Driver Device
    return device;
}

bool GPUDriver::getDriverFeature(GPUDriverFeature feature) {
    bool check = false;
    if (impl__checkDriver())
        check = m_driver->impl__getDriverFeature(feature);
    // Return Driver Feature Check
    return check;
}

GPUDriverOption GPUDriver::getDriverOption() {
    GPUDriverOption option = GPUDriverOption::DRIVER_NONE;
    if (impl__checkDriver())
        option = m_driver->impl__getDriverOption();
    // Return Driver Option
    return option;
}

void GPUDriver::setVerticalSync(bool value) {
    if (impl__checkDriver())
        m_driver->impl__setVerticalSync(value);
}

bool GPUDriver::getVerticalSync() {
    bool check = false;
    if (impl__checkDriver())
        check = m_driver->impl__getVerticalSync();
    // Return Check Vertical Sync
    return check;
}

// -------------------------------
// Context Creation: Window Cached
// -------------------------------

GPUContext* GPUDevice::GPUContextCache::find(void* window) {
    GPUContext* ctx = m_list;
    // Find if there is already a Context
    while (ctx) {
        if (ctx->m_native == window)
            return ctx;
        ctx = ctx->m_next;
    }

    // Return Context
    return ctx;
}

void GPUDevice::GPUContextCache::add(GPUContext* ctx) {
    if (m_list) m_list->m_prev = ctx;
    // Add Context at First
    ctx->m_next = m_list;
    ctx->m_prev = nullptr;
    m_list = ctx;
}

void GPUDevice::GPUContextCache::remove(GPUContext* ctx) {
    if (m_list == ctx)
        m_list = m_list->m_next;
    if (ctx->m_next) ctx->m_next->m_prev = ctx->m_prev;
    if (ctx->m_prev) ctx->m_prev->m_next = ctx->m_next;

    // Detach GPU Context
    ctx->m_next = nullptr;
    ctx->m_prev = nullptr;
}

// -----------------------------------
// Context Creation: Windowing Library
// -----------------------------------

#if defined(NOGPU_GLFW)

GPUContext* GPUDevice::createContext(GLFWwindow *win) {

}

#endif

// ----------------------
// Context Creation: SDL2
// ----------------------

#if defined(NOGPU_SDL2)
#include <SDL2/SDL_syswm.h>

GPUContext* GPUDevice::createContextSDL(SDL_Window *win) {
    SDL_SysWMinfo syswm{};
    // Get SDL2 Native Info
    if (!win || !SDL_GetWindowWMInfo(win, &syswm)) {
        GPUReport::error("invalid SDL2 window %s", SDL_GetError());
        return nullptr;
    }

    // Check SDL2 Window Flags
    if (SDL_GetWindowFlags(win) & (SDL_WINDOW_OPENGL | SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) {
        GPUReport::error("SDL2 window flags must not have SDL_WINDOW_OPENGL | SDL_WINDOW_VULKAN | SDL_WINDOW_METAL");
        return nullptr;
    } else if (SDL_HasWindowSurface(win) || SDL_GetRenderer(win)) {
        GPUReport::error("SDL2 window must not have a SDL_Surface or SDL_Renderer");
        return nullptr;
    }

    // Create Wayland Context
    switch (syswm.subsystem) {
        default: break;

        case SDL_SYSWM_WAYLAND: {
            if (syswm.info.wl.egl_window != nullptr) {
                GPUReport::error("SDL2 window must not have an egl_window");
                return nullptr;
            }

            int w, h; SDL_GetWindowSize(win, &w, &h);
            if (syswm.info.wl.display && syswm.info.wl.surface) {
                GPUWindowWayland window_native;
                window_native.display = syswm.info.wl.display;
                window_native.surface = syswm.info.wl.surface;
                window_native.w = w;
                window_native.h = h;

                // Return Wayland Native Surface
                return createContextWayland(window_native);
            }
        } break;

        case SDL_SYSWM_X11: {
            int w, h; SDL_GetWindowSize(win, &w, &h);
            if (syswm.info.x11.display && syswm.info.x11.window) {
                GPUWindowX11 window_native;
                window_native.display = syswm.info.x11.display;
                window_native.window = syswm.info.x11.window;
                window_native.w = w;
                window_native.h = h;

                // Return Wayland Native Surface
                return createContextX11(window_native);
            }
        } break;
    }

    // Invalid Windowing
    GPUReport::error("SDL2 window is not Wayland or X11");
    return nullptr;
}

#endif

// ----------------------
// Context Creation: SDL3
// ----------------------

#if defined(NOGPU_SDL3)

GPUContext* GPUDevice::createContextSDL(SDL_Window *win) {
    SDL_PropertiesID win_props;
    // Check Valid SDL3 Window
    if (!win || (win_props = SDL_GetWindowProperties(win)) == 0) {
        GPUReport::error("invalid SDL3 window");
        return nullptr;
    }

    // Check SDL3 Window Flags
    int w, h; SDL_GetWindowSize(win, &w, &h);
    if (SDL_GetWindowFlags(win) & (SDL_WINDOW_OPENGL | SDL_WINDOW_VULKAN | SDL_WINDOW_METAL)) {
        GPUReport::error("SDL3 window flags must not have SDL_WINDOW_OPENGL | SDL_WINDOW_VULKAN | SDL_WINDOW_METAL");
        return nullptr;
    } else if (SDL_WindowHasSurface(win) || SDL_GetRenderer(win)) {
        GPUReport::error("SDL3 window must not have a SDL_Surface or SDL_Renderer");
        return nullptr;
    }

    // Try Wayland Surface
    void* display = SDL_GetPointerProperty(win_props,
        SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, nullptr);
    if (display != nullptr) {
        if (SDL_HasProperty(win_props, SDL_PROP_WINDOW_WAYLAND_EGL_WINDOW_POINTER)) {
            GPUReport::error("SDL3 window must not have an egl_window");
            return nullptr;
        }

        void* surface = SDL_GetPointerProperty(win_props,
            SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
        // Create Wayland EGL Context for SDL3 Window
        if (display != nullptr && surface != nullptr) {
            GPUWindowWayland window_native;
            window_native.display = display;
            window_native.surface = surface;
            window_native.w = w;
            window_native.h = h;

            // Return Wayland Native Surface
            return createContextWayland(window_native);
        }
    }

    // Try X11 Surface
    display = SDL_GetPointerProperty(win_props,
        SDL_PROP_WINDOW_X11_DISPLAY_POINTER, nullptr);
    if (display != nullptr) {
        unsigned long xid = SDL_GetNumberProperty(win_props,
            SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
        // Create X11 EGL Context for SDL3 Window
        if (display != nullptr && xid != 0) {
            GPUWindowX11 window_native;
            window_native.display = display;
            window_native.window = xid;
            window_native.w = w;
            window_native.h = h;

            // Return X11 Native Surface
            return createContextX11(window_native);
        }
    }

    // No Valid SDL3 Window Found
    GPUReport::error("SDL3 window is not Wayland or X11");
    return nullptr;
}

#endif // SDL2 & SDL3
