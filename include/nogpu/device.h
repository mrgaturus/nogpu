// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_DEVICE_H
#define NOGPU_DEVICE_H

enum class GPUDebugLevel : int {
    DEBUG_INFO,
    DEBUG_SUCCESS,
    DEBUG_WARNING,
    DEBUG_ERROR,
    DEBUG_ASSERT,
    DEBUG_LOG,
};

typedef struct {
    void* object;
    const char* message;
    int message_size;
    GPUDebugLevel level;
} GPUDebugReport;

typedef void (*GPUDebugCallback)(
    void* userdata, GPUDebugReport report);

// ---------------------------
// GPU Objects: Driver Options
// ---------------------------

enum class GPUDriverMode : int {
    DRIVER_MODE_NORMAL,
    DRIVER_MODE_DANGER,
    DRIVER_MODE_REPORT,
    DRIVER_MODE_LOGGER,
    DRIVER_MODE_DEBUG,
};

enum class GPUDriverOption : int {
    DRIVER_NONE,
    DRIVER_AUTO,
    // Driver Options
    DRIVER_OPENGL,
    DRIVER_VULKAN,
    DRIVER_DX11,
    DRIVER_DX12,
    DRIVER_METAL
};

enum class GPUDeviceOption : int {
    DEVICE_NONE,
    DEVICE_AUTO,
    // Device Options
    DEVICE_X11,
    DEVICE_WAYLAND,
    DEVICE_WIN32,
};

enum class GPUDriverFeature : int {
    DRIVER_FEATURE_RASTERIZE,
    DRIVER_FEATURE_COMPUTE,
    DRIVER_FEATURE_DEBUG,

    // Texture Features
    DRIVER_TEXTURE_1D,
    DRIVER_TEXTURE_BUFFER,
    DRIVER_TEXTURE_RGBA16,
    DRIVER_TEXTURE_CUBEMAP_ARRAY,
    DRIVER_TEXTURE_MULTISAMPLE,
    // Texture Compressed Features
    DRIVER_TEXTURE_COMPRESSED_RGTC,
    DRIVER_TEXTURE_COMPRESSED_DXTC,
    DRIVER_TEXTURE_COMPRESSED_BC7,
    DRIVER_TEXTURE_COMPRESSED_ETC2,
    DRIVER_TEXTURE_COMPRESSED_ASTC,

    // Shader Uniform Blocks
    DRIVER_UNIFORM_BUFFER_OBJECT,
    DRIVER_UNIFORM_SHADER_STORAGE,
    DRIVER_UNIFORM_ATOMIC_COUNTER,
    // Built-in Shader Compiling
    DRIVER_SHADER_GLSL, // OpenGL
    DRIVER_SHADER_SPIRV, // Vulkan
    DRIVER_SHADER_DXBC, // DirectX 11
    DRIVER_SHADER_DXIL, // DirectX 12
    DRIVER_SHADER_MTL, // Metal
};

// ---------------------------
// GPU Device: Native Platform
// ---------------------------

// Include GLFW Window
#if defined(NOGPU_GLFW)
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#endif

// Include SDL Window
#if defined(NOGPU_SDL3)
#include <SDL3/SDL.h>
#elif defined(NOGPU_SDL2)
#include <SDL2/SDL.h>
#endif

#if defined(__unix__)

typedef struct {
    void* display;        // Xlib.h :: Display
    unsigned long window; // Xlib.h :: Window
    int w, h;
} GPUWindowX11;

typedef struct {
    void* display; // wayland-client.h :: wl_display
    void* surface; // wayland-client.h :: wl_surface
    int w, h;
} GPUWindowWayland;

#endif

// -------------------
// GPU Device: Objects
// -------------------

class GPUContext;
class GPUDevice;
class GPUDriver {
    static GPUDriver* m_driver;
    static bool impl__checkDriver();

    protected: // Driver Abstract Methods
        virtual GPUDevice* impl__createDevice(GPUDeviceOption device, int samples, bool rgba) = 0;
        virtual void impl__setVerticalSync(bool value) = 0;
        virtual bool impl__getVerticalSync() = 0;
        virtual bool impl__getDriverFeature(GPUDriverFeature feature) = 0;
        virtual GPUDriverOption impl__getDriverOption() = 0;
        virtual bool impl__shutdown() = 0;

    public: // Driver Preload Methods
        static bool initialize(GPUDriverOption driver, GPUDriverMode mode);
        static GPUDevice* createDevice(GPUDeviceOption device, int samples, bool rgba);
        static void setDebugCallback(GPUDebugCallback cb, void* userdata);
        static void setVerticalSync(bool value);
        static bool getVerticalSync();
        static bool getDriverFeature(GPUDriverFeature feature);
        static GPUDriverOption getDriverOption();
        static bool shutdown();
};

class GPUDevice {
    public: // Basic Device Info
        virtual void setVerticalSync(bool value) = 0;
        virtual bool getVerticalSync() = 0;
        virtual GPUDeviceOption checkOption() = 0;
        virtual int checkSamples() = 0;
        virtual bool checkRGBA() = 0;
        virtual bool destroy() = 0;

    // Context Creation: GLFW
    #if defined(NOGPU_GLFW)
        GPUContext *createContextGLFW(GLFWwindow *win);
    #endif

    // Context Creation: SDL2 & SDL3
    #if defined(NOGPU_SDL2) || defined(NOGPU_SDL3)
        GPUContext *createContextSDL(SDL_Window *win);
    #endif

    // Context Creation: Raw Platform
    #if defined(__unix__)
        virtual GPUContext *createContextX11(GPUWindowX11 win) = 0;
        virtual GPUContext *createContextWayland(GPUWindowWayland win) = 0;
    #endif
};

#endif // NOGPU_DEVICE_H
