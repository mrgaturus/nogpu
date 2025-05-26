// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <cstdio>
#include <nogpu.h>

// ----------------
// SDL3 Hello World
// ----------------

#if defined(NOGPU_SDL3)
#include <SDL3/SDL.h>

int main() {
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) == 0) {
        printf("failed initialize video driver\n");
        return ~0;
    }

    SDL_Window* win = SDL_CreateWindow("nogpu hello", 1024, 600, SDL_WINDOW_HIDDEN);
    GPUDriver::initialize(GPUDriverOption::DRIVER_OPENGL);
    GPUContext* gpuCTX = GPUDriver::createContext(win);
    SDL_ShowWindow(win);

    SDL_Event ev;
    while (true) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) {
                SDL_Quit();
                goto SHUTDOWN_DRIVER;
            }
        }

        // Swap Context Surface
        gpuCTX->swapSurface();
        SDL_Delay(16);
    }

SHUTDOWN_DRIVER:
    GPUDriver::shutdown();
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

    SDL_Window* win = SDL_CreateWindow("nogpu hello",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 600, SDL_WINDOW_HIDDEN);
    GPUDriver::initialize(GPUDriverOption::DRIVER_OPENGL);
    GPUContext* gpuCTX = GPUDriver::createContext(win);
    SDL_ShowWindow(win);

    SDL_Event ev;
    while (true) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                SDL_Quit();
                goto SHUTDOWN_DRIVER;
            }
        }

        // Swap Context Surface
        gpuCTX->swapSurface();
        SDL_Delay(16);
    }

SHUTDOWN_DRIVER:
    GPUDriver::shutdown();
    return 0;
}

#endif
