// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <cstdio>
#include <nogpu.h>

// -----------------
// GLFW3 Hello World
// -----------------

#if defined(NOGPU_GLFW)
#include <GLFW/glfw3.h>

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        return -1;  // Failed to initialize GLFW
    }

    // Create a windowed mode window and its OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1024, 600, "nogpu hello", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;  // Failed to create window
    }

    // Create GPU Driver
    GPUDevice::initialize(GPUDeviceDriver::DRIVER_OPENGL);
    GPUContext* ctx = GPUDevice::createContext(window);
    GPUDevice::setVerticalSync(true);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
       glfwPollEvents();
       ctx->swapSurface();
    }

    // Clean up GLFW
    GPUDevice::shutdown();
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
        printf("failed initialize video driver\n");
        return ~0;
    }

    SDL_Window* win = SDL_CreateWindow("nogpu sdl3 window", 1024, 600,
        SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
    GPUDriver::initialize(GPUDriverOption::DRIVER_OPENGL);
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
        printf("failed initialize video driver\n");
        return ~0;
    }

    SDL_Window* win = SDL_CreateWindow("nogpu sdl2 window",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 600,
        SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE);
    GPUDriver::initialize(GPUDriverOption::DRIVER_OPENGL);
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
