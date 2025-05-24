// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <cstdio>
#include <nogpu.h>
#include <SDL2/SDL.h>

int main() {
    if (SDL_VideoInit(NULL) != 0)
        printf("failed initialize video driver\n");
    SDL_Window* win = SDL_CreateWindow("nogpu triangle",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 600, SDL_WINDOW_HIDDEN);

    GPUDriver::initialize(GPUDriverOption::DRIVER_OPENGL);
    GPUContext* gpuCTX = GPUDriver::createContext(win);
    SDL_ShowWindow(win);
    gpuCTX->swapSurface();

    printf("%s\n", "hello world");
    char hello[1024];
    fgets(hello, 1024, stdin);

    GPUDriver::shutdown();
    return 0;
}
