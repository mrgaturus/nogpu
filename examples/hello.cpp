// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <cstdio>
#include <nogpu.h>

// -----------------
// GLFW3 Hello World
// -----------------

#if defined(NOGPU_GLFW)
#include <GLFW/glfw3.h>

void glfw_windowSizeCallback(GLFWwindow* window, int width, int height) {
    void* userdata = glfwGetWindowUserPointer(window);
    GPUContext* ctx = reinterpret_cast<GPUContext*>(userdata);
    ctx->surfaceResize(width, height);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        printf("failed initialize glfw\n");
        return -1;  // Failed to initialize GLFW
    }

    // Create a windowed mode window and its OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* win = glfwCreateWindow(1024, 600, "nogpu glfw window", NULL, NULL);
    if (!win) {
        printf("failed create glfw window\n");
        glfwTerminate();
        return -1;  // Failed to create window
    }

    GPUDriver::initialize(GPUDriverOption::DRIVER_OPENGL, GPUDriverMode::DRIVER_MODE_DEBUG);
    GPUDriver::setVerticalSync(true);
    // Create GPU Device and Context
    GPUDevice* dev = GPUDriver::createDevice(GPUDeviceOption::DEVICE_AUTO, 0, false);
    GPUContext* ctx = dev->createContextGLFW(win);
    // Configure Resize Callback
    glfwSetWindowUserPointer(win, (void*) ctx);
    glfwSetWindowSizeCallback(win,
        (GLFWwindowsizefun) glfw_windowSizeCallback);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(win)) {
       glfwPollEvents();
       ctx->surfaceSwap();
    }

    // Clean up GLFW
    ctx->destroy();
    dev->destroy();
    GPUDriver::shutdown();
    glfwTerminate();
    return 0;
}

// ----------------
// SDL3 Hello World
// ----------------

#elif defined(NOGPU_SDL3)
#include <SDL3/SDL.h>

int main() {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) == 0) {
        printf("failed initialize sdl3 video\n");
        return ~0;
    }

    SDL_Window* win = SDL_CreateWindow("nogpu sdl3 window", 1024, 600,
        SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
    GPUDriver::initialize(GPUDriverOption::DRIVER_OPENGL, GPUDriverMode::DRIVER_MODE_DEBUG);
    GPUDriver::setVerticalSync(true);
    // Create GPU Device and Context
    GPUDevice* dev = GPUDriver::createDevice(GPUDeviceOption::DEVICE_AUTO, 0, false);
    GPUContext* ctx = dev->createContextSDL(win);
    SDL_ShowWindow(win);

    SDL_Event ev;
    while (true) {
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_EVENT_QUIT:
                    goto SHUTDOWN_DRIVER;
                case SDL_EVENT_WINDOW_RESIZED:
                    ctx->surfaceResize(ev.window.data1, ev.window.data2);
                    break;
            }
        }

        // Swap Context Surface
        ctx->surfaceSwap();
    }

SHUTDOWN_DRIVER:
    ctx->destroy();
    dev->destroy();
    GPUDriver::shutdown();
    SDL_Quit();
    return 0;
}

// ----------------
// SDL2 Hello World
// ----------------

#elif defined(NOGPU_SDL2)
#include <SDL2/SDL.h>

int main() {
    if (SDL_VideoInit(NULL) != 0) {
        printf("failed initialize sdl2 video\n");
        return ~0;
    }

    SDL_Window* win = SDL_CreateWindow("nogpu sdl2 window",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 600,
        SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
    GPUDriver::initialize(GPUDriverOption::DRIVER_OPENGL, GPUDriverMode::DRIVER_MODE_DEBUG);
    GPUDriver::setVerticalSync(true);
    // Create GPU Device and Context
    GPUDevice* dev = GPUDriver::createDevice(GPUDeviceOption::DEVICE_AUTO, 0, false);
    GPUContext* ctx = dev->createContextSDL(win);
    SDL_ShowWindow(win);

    SDL_Event ev;
    while (true) {
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
                case SDL_QUIT:
                    goto SHUTDOWN_DRIVER;
                case SDL_WINDOWEVENT_RESIZED:
                    ctx->surfaceResize(ev.window.data1, ev.window.data2);
                    break;
            }
        }

        // Swap Context Surface
        ctx->surfaceSwap();
    }

SHUTDOWN_DRIVER:
    ctx->destroy();
    dev->destroy();
    GPUDriver::shutdown();
    SDL_Quit();
    return 0;
}

#endif
