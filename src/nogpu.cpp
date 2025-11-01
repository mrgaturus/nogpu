// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <opengl/private/driver.h>
#include <nogpu_private.h>
#include <nogpu.h>

// Global Driver Definition
GPUDriver* GPUDriver::m_driver = nullptr;
bool GPUDriver::initialize(GPUDriverOption driver, GPUDriverMode mode) {
    GPUReport::setMode(mode);
    GPUReport::setObject(nullptr);
    bool result = false;

    if (m_driver) {
        GPUReport::error("driver already initialized");
        return false;
    }

    // XXX: for now OpenGL to whole api battle test
    #if defined(__unix__)
    OPENGL_DRIVER: {
        GLDriver* gl = new GLDriver(mode, result);
        if (result) {
            m_driver = gl;
            return result;
        } else delete gl;
    }
    #endif

    // Check Driver Initialized
    return result;
}

bool GPUDriver::impl__checkDriver() {
    bool check = m_driver != nullptr;
    if (!check) GPUReport::error("driver not initialized");
    // Return Driver Initialized
    return check;
}

bool GPUDriver::shutdown() {
    bool result = (m_driver) &&
        m_driver->impl__shutdown();
    // Remove Current Driver
    m_driver = nullptr;
    return result;
}

// ------------------
// GPU Driver Methods
// ------------------

GPUDevice* GPUDriver::createDevice(GPUDeviceOption option, int samples, bool rgba) {
    GPUDevice* device = nullptr;
    if (impl__checkDriver())
        device = m_driver->impl__createDevice(option, samples, rgba);
    // Return Driver Device
    return device;
}

void GPUDriver::setDebugCallback(GPUDebugCallback cb, void* userdata) {
    GPUReport::setCallback(cb, userdata);
}

void GPUDriver::setVerticalSync(bool value) {
    if (impl__checkDriver())
        m_driver->impl__setVerticalSync(value);
}

bool GPUDriver::getVerticalSync() {
    bool check = false;
    if (impl__checkDriver())
        check = m_driver->impl__getVerticalSync();
    // Return Check Vertical Sync
    return check;
}

bool GPUDriver::getDriverFeature(GPUDriverFeature feature) {
    bool check = false;
    if (impl__checkDriver())
        check = m_driver->impl__getDriverFeature(feature);
    // Return Driver Feature Check
    return check;
}

GPUDriverOption GPUDriver::getDriverOption() {
    GPUDriverOption option = GPUDriverOption::DRIVER_NONE;
    if (impl__checkDriver())
        option = m_driver->impl__getDriverOption();
    // Return Driver Option
    return option;
}

// -------------------------------
// Context Creation: Window Cached
// -------------------------------

GPUContext* GPUContextCache::find(void* window) {
    GPUContext* ctx = m_list;
    // Find if there is already a Context
    while (ctx) {
        if (ctx->m_native == window)
            return ctx;
        ctx = ctx->m_next;
    }

    // Return Context
    return ctx;
}

void GPUContextCache::add(GPUContext* ctx) {
    if (m_list) m_list->m_prev = ctx;
    // Add Context at First
    ctx->m_next = m_list;
    ctx->m_prev = nullptr;
    m_list = ctx;
}

void GPUContextCache::remove(GPUContext* ctx) {
    if (m_list == ctx)
        m_list = m_list->m_next;
    if (ctx->m_next) ctx->m_next->m_prev = ctx->m_prev;
    if (ctx->m_prev) ctx->m_prev->m_next = ctx->m_next;

    // Detach GPU Context
    ctx->m_next = nullptr;
    ctx->m_prev = nullptr;
}
