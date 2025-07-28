// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <opengl/private/context.h>
#include <nogpu_private.h>
#include <nogpu.h>

// Global Driver Definition
GPUDevice* GPUDevice::m_device = nullptr;
bool GPUDevice::initialize(GPUDeviceDriver driver, int msaa_samples, bool rgba) {
    if (m_device) {
        GPUReport::error("driver already initialized");
        return false;
    }

    // XXX: for now OpenGL Linux to design the api before whole abstraction
    #if defined(__unix__)
    OPENGL_DRIVER: {
        GLDriver* gl = new GLDriver(msaa_samples, rgba);
        if (gl) m_device = gl;
        else delete gl;
    }
    #endif

    // Check Driver Initialized
    return !! m_device;
}

bool GPUDevice::shutdown() {
    bool result = (m_device) &&
        m_device->impl__shutdown();
    // Remove Current Driver
    m_device = nullptr;
    return result;
}

// ----------------------
// GPU Driver Information
// ----------------------

bool GPUDevice::checkInitialized() {
    return (m_device) && m_device->impl__checkInitialized();
}

bool GPUDevice::checkRGBASurface() {
    return (m_device) && m_device->impl__checkRGBASurface();
}

bool GPUDevice::checkVerticalSync() {
    return (m_device) && m_device->impl__checkVerticalSync();
}

bool GPUDevice::checkFeature(GPUDeviceFeature feature) {
    return (m_device) && m_device->impl__checkFeature(feature);
}

int GPUDevice::getMultisamplesCount() {
    if (!m_device) return 0;
    return m_device->impl__getMultisamplesCount();
}

GPUDeviceDriver GPUDevice::getDeviceDriver() {
    if (!m_device) return GPUDeviceDriver::DRIVER_NONE;
    return m_device->impl__getDeviceDriver();
}

bool GPUDevice::getVerticalSync() {
    if (!m_device) return false;
    return m_device->impl__getVerticalSync();
}

void GPUDevice::setVerticalSync(bool value) {
    if (m_device) m_device->impl__setVerticalSync(value);
}

// -------------------------------
// Context Creation: Window Cached
// -------------------------------

GPUContext* GPUDevice::cached__find(void* window) {
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

void GPUDevice::cached__add(GPUContext* ctx) {
    if (m_ctx_cache)
        m_ctx_cache->m_prev = ctx;

    // Add Context at First
    ctx->m_next = m_ctx_cache;
    ctx->m_prev = nullptr;
    m_ctx_cache = ctx;
}

void GPUDevice::cached__remove(GPUContext* ctx) {
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

GPUContext* GPUDevice::createContext(GLFWwindow *win) {
    if (!GPUDevice::checkInitialized()) {
        GPUReport::error("driver is not initialized");
        return nullptr;
    }

    // Find or Create New Context
    GPUContext* ctx = m_device->cached__find(win);
    if (!ctx) {
        ctx = m_device->impl__createContext(win);
        m_device->cached__add(ctx);
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

GPUContext* GPUDevice::createContext(SDL_Window *win) {
    if (!GPUDevice::checkInitialized()) {
        GPUReport::error("driver is not initialized");
        return nullptr;
    }

    // Find or Create New Context
    GPUContext* ctx = m_device->cached__find(win);
    if (!ctx) {
        ctx = m_device->impl__createContext(win);
        m_device->cached__add(ctx);
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

GPUContext* GPUDevice::createContext(GPUWindowX11 win) {
    if (!GPUDevice::checkInitialized()) {
        GPUReport::error("driver is not initialized");
        return nullptr;
    }

    // Find or Create New Context
    void* win0 = (void*) win.window;
    GPUContext* ctx = m_device->cached__find(win0);
    if (!ctx) {
        ctx = m_device->impl__createContext(win);
        m_device->cached__add(ctx);
        ctx->m_window = win0;
    }

    // Return Context
    return ctx;
}

GPUContext* GPUDevice::createContext(GPUWindowWayland win) {
    if (!GPUDevice::checkInitialized()) {
        GPUReport::error("driver is not initialized");
        return nullptr;
    }

    // Find or Create New Context
    GPUContext* ctx = m_device->cached__find(win.surface);
    if (!ctx) {
        ctx = m_device->impl__createContext(win);
        m_device->cached__add(ctx);
        ctx->m_window = win.surface;
    }

    // Return Context
    return ctx;
}

#endif // Raw Platform
