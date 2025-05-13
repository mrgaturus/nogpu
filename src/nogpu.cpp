// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu.h>

GPUDriverOption GPUDriver::getDriverOption() {
    return m_driver_option;
}

bool GPUDriver::initialize(GPUDriverOption option) {
    return false;
}

// ----------------
// Context Checking
// ----------------

bool GPUDriver::checkFeature(GPUDriverFeature feature) {
    return m_driver->impl__checkFeature(feature);
}

bool GPUDriver::checkInitialized() {
    return (m_driver) && m_driver->impl__checkInitialized();
}

bool GPUDriver::shutdown() {
    return m_driver->impl__shutdown();
}

// -----------------------------
// Context Creation: SDL2 & SDL3
// -----------------------------

#if defined(NOGPU_SDL2) || defined(NOGPU_SDL3)
GPUContext* GPUDriver::createContext(SDL_Window *win) {
    if (!m_driver) return nullptr;
    return m_driver->impl__createContext(win);
}
#endif // SDL2 & SDL3
