// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_DRIVER_H
#define NOGPU_DRIVER_H

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

// -------------------------
// GPU Objects: Window Linux
// -------------------------

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
// GPU Objects: Driver
// -------------------

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

    // Shader Compiling
    DRIVER_SHADER_GLSL,
    DRIVER_SHADER_HLSL,
    DRIVER_SHADER_SPIRV,
    // Built-in Shader Compiling
    DRIVER_SHADER_LOW_GLSL, // OpenGL
    DRIVER_SHADER_LOW_SPIRV, // Vulkan
    DRIVER_SHADER_LOW_DXBC, // DirectX 11
    DRIVER_SHADER_LOW_DXIL, // DirectX 12
    DRIVER_SHADER_LOW_MTL, // Metal
};

class GPUContext;
class GPUDriver {
    protected:
        GPUContext* m_ctx_cache = nullptr;
        GPUContext* cached__find(void* window);
        void cached__add(GPUContext* ctx);
        void cached__remove(GPUContext* ctx);

        static GPUDriver *m_driver;
        static thread_local GPUDriver *m_driver_lock;
        static unsigned int feature__flag(GPUDriverFeature feature) {
            return ((unsigned int) 1 << (unsigned int) feature);
        }

        // Driver Abstract Implementation
        virtual bool impl__shutdown() = 0;
        virtual bool impl__checkInitialized() = 0;
        virtual bool impl__checkRGBASurface() = 0;
        virtual bool impl__checkVerticalSync() = 0;
        virtual bool impl__checkFeature(GPUDriverFeature feature) = 0;
        virtual int impl__getMultisamplesCount() = 0;
        virtual GPUDriverOption impl__getDriverOption() = 0;
        virtual void impl__setVerticalSync(bool value) = 0;

    public: // Driver Initialize
        static bool initialize(GPUDriverOption option,
            int msaa_samples = 0, bool rgba = false);
        static bool shutdown();
        // Driver Information
        static bool checkInitialized();
        static bool checkRGBASurface();
        static bool checkVerticalSync();
        static bool checkFeature(GPUDriverFeature feature);
        static int getMultisamplesCount();
        static GPUDriverOption getDriverOption();
        static void setVerticalSync(bool value);

    // -- GPU Driver: Context Creation --

    // Context Creation: GLFW
    #if defined(NOGPU_GLFW)
        protected: virtual GPUContext *impl__createContext(GLFWwindow *win) = 0;
        public: static GPUContext *createContext(GLFWwindow *win);
    #endif

    // Context Creation: SDL2 & SDL3
    #if defined(NOGPU_SDL2) || defined(NOGPU_SDL3)
        protected: virtual GPUContext *impl__createContext(SDL_Window *win) = 0;
        public: static GPUContext *createContext(SDL_Window *win);
    #endif

    // Context Creation: Raw Platform
    #if defined(__unix__)
        protected: virtual GPUContext *impl__createContext(GPUWindowX11 win) = 0;
        protected: virtual GPUContext *impl__createContext(GPUWindowWayland win) = 0;
        public: static GPUContext *createContext(GPUWindowX11 win);
        public: static GPUContext *createContext(GPUWindowWayland win);
    #endif
};

#endif // NOGPU_DRIVER_H
