// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include <nogpu.h>

// ----------------------
// Context Creation: GLFW
// ----------------------

#if defined(NOGPU_GLFW)

extern "C" {
    extern GLFWAPI void* glfwGetX11Display(void);
    extern GLFWAPI void* glfwGetWaylandDisplay(void);
    extern GLFWAPI unsigned long glfwGetX11Window(GLFWwindow* window);
    extern GLFWAPI void* glfwGetWaylandWindow(GLFWwindow* window);
    extern GLFWAPI void* glfwGetGLXContext(GLFWwindow* window);
    extern GLFWAPI void* glfwGetEGLContext(GLFWwindow* window);
}

GPUContext* GPUDevice::createContextGLFW(GLFWwindow *win) {
    if (win == nullptr) {
        GPUReport::error("invalid GLFW window");
        return nullptr;
    } else if (glfwGetGLXContext(win) || glfwGetEGLContext(win)) {
        GPUReport::error("GLFW window must not have a GLX context or EGL context");
        return nullptr;
    } else if (glfwGetWindowAttrib(win, GLFW_CLIENT_API) != GLFW_NO_API) {
        GPUReport::error("GLFW window hint GLFW_CLIENT_API must be GLFW_NO_API");
        return nullptr;
    }

    // Try Wayland Surface
    void* display = glfwGetWaylandDisplay();
    if (display != nullptr) {
        void* surface = glfwGetWaylandWindow(win);
        if (display != nullptr && surface != nullptr) {
            int w, h; glfwGetWindowSize(win, &w, &h);
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
    display = glfwGetX11Display();
    if (display != nullptr) {
        unsigned long xid = glfwGetX11Window(win);
        if (display != nullptr && xid != 0) {
            int w, h; glfwGetWindowSize(win, &w, &h);
            GPUWindowX11 window_native;
            window_native.display = display;
            window_native.window = xid;
            window_native.w = w;
            window_native.h = h;

            // Return X11 Native Surface
            return createContextX11(window_native);
        }
    }

    // No Valid GLFW3 Window Found
    GPUReport::error("GLFW window is not Wayland or X11");
    return nullptr;
}

#endif // GLFW

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

// ----------------------
// Context Creation: SDL2
// ----------------------

#elif defined(NOGPU_SDL2)
#include <SDL2/SDL_syswm.h>

GPUContext* GPUDevice::createContextSDL(SDL_Window *win) {
    SDL_SysWMinfo syswm{};
    // Get SDL2 Native Info
    if (win == nullptr) {
        GPUReport::error("invalid SDL2 window");
        return nullptr;
    } else if (!SDL_GetWindowWMInfo(win, &syswm)) {
        GPUReport::error("failed getting SDL2 syswm: %s", SDL_GetError());
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

                // Return X11 Native Surface
                return createContextX11(window_native);
            }
        } break;
    }

    // Invalid Windowing
    GPUReport::error("SDL2 window is not Wayland or X11");
    return nullptr;
}

#endif // SDL
