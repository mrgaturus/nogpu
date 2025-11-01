// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef OPENGL_DRIVER_H
#define OPENGL_DRIVER_H
#include <nogpu/device.h>
#include <nogpu_private.h>
#include "state.hpp"

#if defined(__unix__)
#include <EGL/egl.h>

enum class LinuxEGLOption : EGLenum {
    LINUX_NONE = 0x0000,
    LINUX_AUTO = 0xFFFF,
    LINUX_WAYLAND = 0x31D8,
    LINUX_X11 = 0x31D5
};

typedef struct LinuxEGLDriver {
    void* so_wayland;
    void* so_x11;
    // EGL Instance
    struct LinuxEGLDevice* list;
    struct LinuxEGLDevice* device;
} LinuxEGLDriver;

typedef struct LinuxEGLDevice {
    struct LinuxEGLDevice* next;
    struct LinuxEGLDevice* prev;
    // EGL Attributes
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    LinuxEGLOption option;
    void* nogpu_device;
    void* nogpu_display;
} LinuxEGLDevice;

typedef struct LinuxEGLContext {
    LinuxEGLDriver* driver;
    LinuxEGLDevice* device;
    // EGL Attributes
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    void* wl_surface;
    void* wl_resize_proc;
    void* wl_destroy_proc;
    void* wl_dimensions_proc;
    // EGL Linux Features
    bool linux_is_x11;
    bool linux_is_rgba;
} LinuxEGLContext;

#endif // __unix__

// -------------
// OpenGL Driver
// -------------

class GLDevice;
class GLContext;
class GLDriver : public GPUDriver {
    #if defined(__unix__)
        LinuxEGLDriver m_egl_driver{};
        bool prepareDevice(GLDevice* device, GPUDeviceOption option);
        bool disposeDevice(GLDevice* device);
    #endif // __unix__

    GLContext* m_ctx_current = nullptr;
    GPUDriverMode m_mode = GPUDriverMode::DRIVER_MODE_NORMAL;
    unsigned int m_features = 0;
    bool m_vsync = true;

    GPUDevice* impl__createDevice(GPUDeviceOption option, int samples, bool rgba) override;
    void impl__setVerticalSync(bool value) override;
    bool impl__getVerticalSync() override;
    bool impl__getDriverFeature(GPUDriverFeature feature) override;
    GPUDriverOption impl__getDriverOption() override;
    bool impl__shutdown() override;

    protected:
        GLDriver(GPUDriverMode mode, bool &result);
        static unsigned int initializeGL(void* getProcAddress);
        void makeCurrent(GLContext* ctx);
        friend GPUDriver;
        friend GLContext;
        friend GLDevice;
};

// -------------
// OpenGL Device
// -------------

class GLDevice : public GPUDevice {
    #if defined(__unix__)
        LinuxEGLDevice m_egl_device{};
    #endif // __unix__

    GPUContextCache m_ctx_cache{};
    GPUDeviceOption m_option;
    GLDriver* m_driver;
    GLState m_state;
    int m_samples;
    bool m_rgba;
    bool m_vsync;

    // Basic Device Info
    void setVerticalSync(bool value) override;
    bool getVerticalSync() override;
    GPUDeviceOption checkOption() override;
    int checkSamples() override;
    bool checkRGBA() override;
    bool destroy() override;

    // Context Creation: Linux
    #if defined(__unix__)
        bool createContextEGL(void* display, LinuxEGLOption device);
        GPUContext *createContextX11(GPUWindowX11 win) override;
        GPUContext *createContextWayland(GPUWindowWayland win) override;
    #endif

    protected: // OpenGL Device Constructor
        GLDevice(GLDriver* driver, GPUDeviceOption device, int samples, bool rgba);
        void prepareContextDebug(GPUDriverMode mode);
        void prepareContextState();

        friend GLDriver;
        friend GLContext;
        friend class GLPipeline;
        friend class GLCommands;
};

#endif // OPENGL_DRIVER_H
