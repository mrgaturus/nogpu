// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu.h>

GPUDriverOption GPUDriver::getDriverOption() {
    if (!m_driver) return GPUDriverOption::DRIVER_NONE;
    return m_driver->impl__getDriverOption();
}

bool GPUDriver::initialize(GPUDriverOption option, int msaa_samples, bool rgba) {
    return false;
}

// ----------------
// Context Checking
// ----------------

bool GPUDriver::checkFeature(GPUDriverFeature feature) {
    return (m_driver) && m_driver->impl__checkFeature(feature);
}

bool GPUDriver::checkInitialized() {
    return (m_driver) && m_driver->impl__checkInitialized();
}

int GPUDriver::getMultisamplesCount() {
    if (!m_driver) return 0;
    return m_driver->impl__getMultisamplesCount();
}

bool GPUDriver::getTransparency() {
    if (!m_driver) return 0;
    return m_driver->impl__getTransparency();
}

bool GPUDriver::shutdown() {
    return (m_driver) && m_driver->impl__shutdown();
}

// -------------------------------
// Context Creation: Window Cached
// -------------------------------

GPUContext* GPUDriver::cached__find(void* window) {
    GPUContext* ctx = m_ctx_cache;
    // Find if there is already a Context
    while (ctx) {
        if (ctx->m_window == window)
            return ctx;
        ctx = ctx->m_next;
    }

    // Return Context
    return ctx;
}

void GPUDriver::cached__add(GPUContext* ctx) {
    if (!m_ctx_cache)
        m_ctx_cache->m_prev = ctx;

    // Add Context at First
    ctx->m_next = m_ctx_cache;
    ctx->m_prev = nullptr;
    m_ctx_cache = ctx;
}

void GPUDriver::cached__remove(GPUContext* ctx) {
    if (m_ctx_cache == ctx)
        m_ctx_cache = m_ctx_cache->m_next;
    if (ctx->m_next) ctx->m_next->m_prev = ctx->m_prev;
    if (ctx->m_prev) ctx->m_prev->m_next = ctx->m_next;

    // Detach GPU Context
    ctx->m_next = nullptr;
    ctx->m_prev = nullptr;
}

// -----------------------------
// Context Creation: SDL2 & SDL3
// -----------------------------

#if defined(NOGPU_SDL2) || defined(NOGPU_SDL3)
GPUContext* GPUDriver::createContext(SDL_Window *win) {
    if (!m_driver) return nullptr;
    // Find or Create New Context
    GPUContext* ctx = m_driver->cached__find(win);
    if (!ctx) ctx = m_driver->impl__createContext(win);
    return ctx;
}
#endif // SDL2 & SDL3
