// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <opengl/private/context.h>
#include <nogpu_private.h>
#include <nogpu.h>
// Global Driver Definition
GPUDriver* GPUDriver::m_driver = nullptr;
thread_local GPUDriver* GPUDriver::m_driver_lock = nullptr;

bool GPUDriver::initialize(GPUDriverOption option, int msaa_samples, bool rgba) {
    if (m_driver) {
        GPULogger::error("driver already initialized");
        return false;
    }

    // XXX: for now OpenGL Linux to design the api before whole abstraction
    #if defined(__unix__)
    OPENGL_DRIVER: {
        GLDriver* gl = new GLDriver(msaa_samples, rgba);
        if (gl) m_driver = gl;
        else delete gl;
    }
    #endif

    // Check Driver Initialized
    return !! m_driver;
}

bool GPUDriver::shutdown() {
    bool result = (m_driver) &&
        m_driver->impl__shutdown();
    // Remove Current Driver
    m_driver = nullptr;
    return result;
}

// ----------------------
// GPU Driver Information
// ----------------------

bool GPUDriver::checkInitialized() {
    return (m_driver) && m_driver->impl__checkInitialized();
}

bool GPUDriver::checkRGBASurface() {
    return (m_driver) && m_driver->impl__checkRGBASurface();
}

bool GPUDriver::checkVerticalSync() {
    return (m_driver) && m_driver->impl__checkVerticalSync();
}

bool GPUDriver::checkFeature(GPUDriverFeature feature) {
    return (m_driver) && m_driver->impl__checkFeature(feature);
}

int GPUDriver::getMultisamplesCount() {
    if (!m_driver) return 0;
    return m_driver->impl__getMultisamplesCount();
}

GPUDriverOption GPUDriver::getDriverOption() {
    if (!m_driver) return GPUDriverOption::DRIVER_NONE;
    return m_driver->impl__getDriverOption();
}

void GPUDriver::setVerticalSync(bool value) {
    if (m_driver) m_driver->impl__setVerticalSync(value);
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
    if (m_ctx_cache)
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

// ----------------------
// Context Creation: GLFW
// ----------------------

#if defined(NOGPU_GLFW)

GPUContext* GPUDriver::createContext(GLFWwindow *win) {
    if (!GPUDriver::checkInitialized()) {
        GPULogger::error("driver is not initialized");
        return nullptr;
    }

    // Find or Create New Context
    GPUContext* ctx = m_driver->cached__find(win);
    if (!ctx) {
        ctx = m_driver->impl__createContext(win);
        m_driver->cached__add(ctx);
        ctx->m_window = win;
    }

    // Return Context
    return ctx;
}

#endif

// -----------------------------
// Context Creation: SDL2 & SDL3
// -----------------------------

#if defined(NOGPU_SDL2) || defined(NOGPU_SDL3)

GPUContext* GPUDriver::createContext(SDL_Window *win) {
    if (!GPUDriver::checkInitialized()) {
        GPULogger::error("driver is not initialized");
        return nullptr;
    }

    // Find or Create New Context
    GPUContext* ctx = m_driver->cached__find(win);
    if (!ctx) {
        ctx = m_driver->impl__createContext(win);
        m_driver->cached__add(ctx);
        ctx->m_window = win;
    }

    // Return Context
    return ctx;
}

#endif // SDL2 & SDL3

// ------------------------------
// Context Creation: Raw Platform
// ------------------------------

#if defined(__unix__)

GPUContext* GPUDriver::createContext(GPUWindowX11 win) {
    if (!GPUDriver::checkInitialized()) {
        GPULogger::error("driver is not initialized");
        return nullptr;
    }

    // Find or Create New Context
    void* win0 = (void*) win.window;
    GPUContext* ctx = m_driver->cached__find(win0);
    if (!ctx) {
        ctx = m_driver->impl__createContext(win);
        m_driver->cached__add(ctx);
        ctx->m_window = win0;
    }

    // Return Context
    return ctx;
}

GPUContext* GPUDriver::createContext(GPUWindowWayland win) {
    if (!GPUDriver::checkInitialized()) {
        GPULogger::error("driver is not initialized");
        return nullptr;
    }

    // Find or Create New Context
    GPUContext* ctx = m_driver->cached__find(win.surface);
    if (!ctx) {
        ctx = m_driver->impl__createContext(win);
        m_driver->cached__add(ctx);
        ctx->m_window = win.surface;
    }

    // Return Context
    return ctx;
}

#endif // Raw Platform
