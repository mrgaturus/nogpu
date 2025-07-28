// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_DRIVER_H
#define OPENGL_DRIVER_H
#include <nogpu/driver.h>

#if defined(__unix__)
#include <EGL/egl.h>

typedef struct LinuxEGL {
    struct LinuxEGL* next;
    struct LinuxEGL* prev;
    // EGL Attributes
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    void* linux_display;
    bool linux_is_x11;
} LinuxEGL;

typedef struct LinuxEGLDriver {
    void* so_wayland;
    void* so_x11;
    // EGL Instance
    LinuxEGL* list;
    LinuxEGL* current;
    EGLSurface surface;
} LinuxEGLDriver;

typedef struct LinuxEGLContext {
    LinuxEGLDriver* driver;
    LinuxEGL* egl;
    // EGL Attributes
    EGLDisplay display;
    EGLSurface surface;
    void* wl_surface;
    void* wl_resize_proc;
    void* wl_destroy_proc;
    void* wl_dimensions_proc;
    // EGL Linux Features
    bool linux_is_x11;
    bool linux_is_rgba;
} LinuxEGLContext;

#endif // defined(__unix__)

class GLContext;
class GLDriver : GPUDevice {
    GLContext* m_context_list = nullptr;
    unsigned int m_features = 0;
    int m_msaa_samples = 0;
    bool m_rgba = false;
    bool m_vsync = false;

    // Linux EGL Context
    #if defined(__unix__)
        LinuxEGLDriver m_egl = {};
        GPUContext* makeLinuxContext(LinuxEGLContext ctx);
    #endif

    bool impl__shutdown() override;
    bool impl__checkInitialized() override;
    bool impl__checkRGBASurface() override;
    bool impl__checkVerticalSync() override;
    bool impl__checkFeature(GPUDeviceFeature feature) override;
    GPUDeviceDriver impl__getDeviceDriver() override;
    int impl__getMultisamplesCount() override;
    bool impl__getVerticalSync() override;
    void impl__setVerticalSync(bool value) override;

    // Context Creation: GLFW
    #if defined(NOGPU_GLFW)
        GPUContext *impl__createContext(GLFWwindow* win) override;
    #endif

    // Context Creation: SDL2 & SDL3
    #if defined(NOGPU_SDL2) || defined(NOGPU_SDL3)
        GPUContext *impl__createContext(SDL_Window *win) override;
    #endif

    // Context Creation: Raw Platform
    #if defined(__unix__)
        GPUContext *impl__createContext(GPUWindowX11 win) override;
        GPUContext *impl__createContext(GPUWindowWayland win) override;
    #endif

    public: // GL Context Current
        void makeCurrent(GLContext* ctx);
        void makeDestroyed(GLContext* ctx);
    protected: // GL Driver Initialize
        GLDriver(int msaa_samples, bool rgba);
        friend GPUDevice;
};

#endif // OPENGL_DRIVER_H