// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include "private/driver.h"
#include "private/context.h"
#include "private/glad.h"
// Include from C
#include <cstdlib>
#include <dlfcn.h>

static EGLint egl_attribs_dummy[] = {
    EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
    EGL_NONE
};

static EGLint egl_attribs_config[] = {
    EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
    EGL_DEPTH_SIZE, 24,
    EGL_STENCIL_SIZE, 8,
    // EGL Color Channels
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 0,
    EGL_BUFFER_SIZE, 24,
    // Optional MSAA
    EGL_SAMPLES, 0,
    EGL_SAMPLE_BUFFERS, 0,
    EGL_NONE
};

static EGLint egl_attribs_context[] = {
    EGL_CONTEXT_MAJOR_VERSION, 3,
    EGL_CONTEXT_MINOR_VERSION, 3,
    EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
    EGL_CONTEXT_OPENGL_DEBUG, EGL_FALSE,
    EGL_NONE
};

static EGLint egl_attribs_surface[] = {
    EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
    EGL_NONE
};

// ---------------------------------------
// Linux OpenGL Device: Attributes Prepare
// ---------------------------------------

static void prepare__egl_attribs_config(int msaa_samples, bool rgba) {
    if (msaa_samples < 0) msaa_samples = 0;
    if (msaa_samples > 16) msaa_samples = 16;

   // Configure RGBA Surface
    if (rgba) {
        egl_attribs_config[13] = 8;
        egl_attribs_config[15] = 32;
    } else {
        egl_attribs_config[13] = 0;
        egl_attribs_config[15] = 24;
    }

    // Configure Multisample Rendering
    egl_attribs_config[17] = (msaa_samples > 0) ? EGL_TRUE : EGL_FALSE;
    egl_attribs_config[19] = next_power_of_two(msaa_samples);
}

static void prepare__egl_attribs_context(int major, int minor, bool debug) {
    egl_attribs_context[1] = major;
    egl_attribs_context[3] = minor;

    // Prepare OpenGL Debug Context
    egl_attribs_context[7] = debug ?
        EGL_TRUE : EGL_FALSE;
}

// --------------------------------
// Linux OpenGL Device: Constructor
// --------------------------------

GLDriver::GLDriver() {
    EGLDisplay egl_display;
    EGLConfig egl_config;
    EGLContext egl_context;
    int egl_num_config;
    int egl_major, egl_minor;

    // Use OpenGL API for EGL
    if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE) {
        GPUReport::error("[opengl] failed initialize OpenGL API");
        return;
    }
    // Create Dummy Display to Check OpenGL
    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (egl_display == EGL_NO_DISPLAY) {
        GPUReport::error("[opengl] failed create EGL display");
        return;
    }

    // Initialize and Configure Display
    if (eglInitialize(egl_display, &egl_major, &egl_minor) == EGL_FALSE) {
        GPUReport::error("[opengl] failed initialize EGL");
        goto TERMINATE_EGL;
    }
    if (eglChooseConfig(egl_display, egl_attribs_dummy, &egl_config, 1, &egl_num_config) == EGL_FALSE) {
        GPUReport::error("[opengl] failed configure EGL");
        goto TERMINATE_EGL;
    }

    // Create OpenGL Dummy Context
    prepare__egl_attribs_context(3, 3, false);
    egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, egl_attribs_context);
    if (egl_context == EGL_NO_CONTEXT) {
        prepare__egl_attribs_context(2, 0, false);
        egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, egl_attribs_context);
    }

    // Check if is Supported
    if (egl_context == EGL_NO_CONTEXT) {
        GPUReport::error("[opengl] device doesn't support OpenGL :C");
        goto TERMINATE_EGL;
    }
    if (eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, egl_context) == EGL_FALSE) {
        GPUReport::error("[opengl] failed check OpenGL features");
        goto TERMINATE_EGL;
    }

    // Initialize OpenGL Functions
    m_features = GLDriver::initializeGL((void*) eglGetProcAddress);
    if (m_features > 0) {
        const char* vendor = (const char*) glGetString(0x1F02);
        GPUReport::success("[opengl] EGL version: %d.%d", egl_major, egl_minor);
        GPUReport::success("[opengl] OpenGL version: %s", vendor);
    }

TERMINATE_EGL:
    eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglTerminate(egl_display);
}

bool GLDriver::impl__shutdown() {
    bool result = true;
    // Shutdown EGL Devices
    LinuxEGLDriver *driver = &m_egl_driver;
    LinuxEGLDevice *device = driver->list;
    while (device) {
        LinuxEGLDevice* next = device->next;
        result &= eglTerminate(device->display);
        free(device); device = device->next;
    }

    // Unload Native Modules and Clear Attributes
    if (driver->so_wayland) dlclose(driver->so_wayland);
    if (driver->so_x11) dlclose(driver->so_x11);
    driver->so_wayland = nullptr;
    driver->so_x11 = nullptr;
    driver->list = nullptr;
    driver->device = nullptr;
    // Reset Private
    m_features = 0;
    m_vsync = false;
    delete this;

    // Return Driver Shutdown Status
    if (result) GPUReport::success("[opengl] terminated EGL display");
    else GPUReport::error("[opengl] failed terminating EGL & OpenGL");
    return result;
}

// ------------------------------
// Linux OpenGL Context: Features
// ------------------------------

GPUDevice* GLDriver::impl__createDevice(GPUDeviceOption device, int samples, bool rgba) {
    return new GLDevice(this, device, samples, rgba);
}

void GLDriver::impl__setVerticalSync(bool value) {
    m_vsync = value;
}

bool GLDriver::impl__getVerticalSync() {
    return m_vsync;
}

bool GLDriver::impl__getDriverFeature(GPUDriverFeature feature) {
    return (m_features & driver_feature_flag(feature)) != 0;
}

GPUDriverOption GLDriver::impl__getDriverOption() {
    return GPUDriverOption::DRIVER_OPENGL;
}

// ----------------------------------
// Linux OpenGL Context: EGL Creation
// ----------------------------------

bool GLDriver::prepareDevice(GLDevice* device, GPUDeviceOption option) {
    LinuxEGLDriver *driver = &m_egl_driver;
    LinuxEGLDevice *egl = &device->m_egl_device;

    // Prepare EGL Option
    LinuxEGLOption egl_option;
    switch (option) {
        case GPUDeviceOption::DEVICE_AUTO:
            egl_option = LinuxEGLOption::LINUX_AUTO;
            break;
        case GPUDeviceOption::DEVICE_X11:
            egl_option = LinuxEGLOption::LINUX_X11;
            break;
        case GPUDeviceOption::DEVICE_WAYLAND:
            egl_option = LinuxEGLOption::LINUX_WAYLAND;
            break;

        default: // Invalid Device Option
            egl_option = LinuxEGLOption::LINUX_NONE;
            GPUReport::error("invalid device option");
            return false;
    }

    // Configure Initial EGL
    egl->option = egl_option;
    egl->nogpu_device = (void*) device;
    egl->nogpu_display = nullptr;
    // Connect Device to List
    if (driver->list != nullptr)
        driver->list->prev = egl;
    egl->next = driver->list;
    driver->list = egl;
    return true;
}

bool GLDriver::disposeDevice(GLDevice* device) {
    LinuxEGLDriver *driver = &m_egl_driver;
    LinuxEGLDevice *egl = &device->m_egl_device;

    if (driver->list == egl)
        driver->list = egl->next;
    // Detach Device from List
    if (egl->next != nullptr)
        egl->next->prev = egl->prev;
    if (egl->prev != nullptr)
        egl->prev->next = egl->next;

    // Dispose EGL Objects
    if (egl->context != EGL_NO_CONTEXT)
        eglDestroyContext(egl->display, egl->context);
    return eglTerminate(egl->display) == EGL_TRUE;
}

// -------------------------------------
// Linux OpenGL Context: Device Creation
// -------------------------------------

GLDevice::GLDevice(GLDriver* driver, GPUDeviceOption option, int samples, bool rgba) {
    if (!driver->prepareDevice(this, option)) {
        delete this;
        return;
    }

    m_vsync = driver->m_vsync;
    // Define Device Attributes
    m_option = option;
    m_driver = driver;
    m_samples = samples;
    m_rgba = rgba;
}

GPUDeviceOption GLDevice::checkOption() {
    return m_option;
}

int GLDevice::checkSamples() {
    return m_samples;
}

bool GLDevice::checkRGBA() {
    return m_rgba;
}

bool GLDevice::destroy() {
    if (m_ctx_cache.m_list != nullptr) {
        GPUReport::error("there are still some gpu contexts alive");
        return false;
    }

    // Return Device Destroyed
    m_driver->disposeDevice(this);
    delete this;
    return true;
}

// -----------------------------------
// Linux OpenGL Context: Vertical Sync
// -----------------------------------

void GLDevice::setVerticalSync(bool value) {
    // Backup Current EGL Objects
    EGLDisplay egl_dpy0 = eglGetCurrentDisplay();
    EGLContext egl_ctx0 = eglGetCurrentContext();
    EGLSurface egl_draw0 = eglGetCurrentSurface(EGL_DRAW);
    EGLSurface egl_read0 = eglGetCurrentSurface(EGL_READ);

    // Apply Vertical Sync to Contexts
    GLContext* ctx = (GLContext*) m_ctx_cache.m_list;
    while (ctx != nullptr) {
        LinuxEGLContext* gtx = &ctx->m_egl_context;
        eglMakeCurrent(gtx->display, gtx->surface, gtx->surface, gtx->context);
        eglSwapInterval(gtx->display, (value) ? EGL_TRUE : EGL_FALSE);
        ctx = (GLContext*) ctx->m_next;
    }

    // Restore Current EGL Objects
    eglMakeCurrent(egl_dpy0, egl_draw0, egl_read0, egl_ctx0);
    m_vsync = value;
}

bool GLDevice::getVerticalSync() {
    return m_vsync;
}

// ----------------------------------------
// Linux OpenGL Context: EGL Initialization
// ----------------------------------------

bool GLDevice::createContextEGL(void* display, LinuxEGLOption option) {
    LinuxEGLDevice *egl = &m_egl_device;
    // Check GPU Device was Initialized for Option
    if (egl->option == LinuxEGLOption::LINUX_AUTO)
        egl->option = option;
    else if (option != egl->option) {
        if (egl->option == LinuxEGLOption::LINUX_X11)
            GPUReport::error("gpu device is reserved for x11");
        else if (egl->option == LinuxEGLOption::LINUX_WAYLAND)
            GPUReport::error("gpu device is reserved for wayland");
        // GL Device Context not Created
        return false;
    }

    // Check Current Display
    if (egl->nogpu_display != nullptr) {
        if (egl->nogpu_display == display)
            return true;
        // EGL Context Display is not the same
        GPUReport::error("window display mismatch for gpu device");
        return false;
    }

    EGLenum egl_option = (EGLenum) egl->option;
    int egl_num_config;
    int egl_major, egl_minor;
    // Create Native Specific EGL Display
    egl->display = eglGetPlatformDisplay(egl_option, display, nullptr);
    if (egl->display == EGL_NO_DISPLAY) {
        GPUReport::error("[opengl] failed create EGL display");
        return false;
    }

    // Initialize EGL Display
    if (eglInitialize(egl->display, &egl_major, &egl_minor) == EGL_FALSE) {
        GPUReport::error("[opengl] failed initialize EGL device");
        goto TERMINATE_EGL;
    }
    // Choose EGL Display Config
    prepare__egl_attribs_config(m_samples, m_rgba);
    if (eglChooseConfig(egl->display, egl_attribs_config, &egl->config, 1, &egl_num_config) == EGL_FALSE) {
        GPUReport::error("[opengl] failed choose EGL config");
        goto TERMINATE_EGL;
    }

    // Create EGL Native Context
    egl->context = eglCreateContext(egl->display, egl->config, EGL_NO_CONTEXT, egl_attribs_context);
    if (egl->context == EGL_NO_CONTEXT) {
        GPUReport::error("[opengl] failed create EGL context");
        goto TERMINATE_EGL;
    }

    return true;
// Terminate Display
TERMINATE_EGL:
    eglTerminate(egl->display);
    return false;
}

// ---------------------------------
// Linux OpenGL Context: X11 Context
// ---------------------------------

typedef struct {
	int x, y;
	int width, height;
	int egl_depth;
	int depth;
    // Unused X11 Attributes
    char unused[232];
} x11_window_attributes_t;

typedef void (*x11_XGetWindowAttributes_t)(
    void* display,
    unsigned long window,
    x11_window_attributes_t* result
);

typedef void (*x11_XResizeWindow_t)(
    void* display,
    unsigned long window,
    unsigned int w,
    unsigned int h
);

GPUContext* GLDevice::createContextX11(GPUWindowX11 win) {
    LinuxEGLDriver *driver = &m_driver->m_egl_driver;
    LinuxEGLDevice *device = &m_egl_device;
    if (!createContextEGL(win.display, LinuxEGLOption::LINUX_X11))
        return nullptr;

    // Open X11 Module
    if (!driver->so_x11) driver->so_x11 =
        dlopen("libX11.so.6", RTLD_LAZY | RTLD_GLOBAL);
    if (!driver->so_x11) {
        GPUReport::error("[opengl] failed load libX11.so.6");
        return nullptr;
    }

    x11_window_attributes_t attribs;
    x11_XGetWindowAttributes_t XGetWindowAttributes =
        (x11_XGetWindowAttributes_t) dlsym(driver->so_x11, "XGetWindowAttributes");
    // Get X11 & EGL Relevant Attributes
    XGetWindowAttributes(win.display, (unsigned long) win.window, &attribs);
    eglGetConfigAttrib(device->display, device->config, EGL_BUFFER_SIZE, &attribs.egl_depth);
    // Check if Window is Transparent
    // WEIRD BEHAVIOUR: x11 window depth forces EGL depth
    //                  so EGL depth is quite useless for X11
    if (attribs.depth != attribs.egl_depth)
        GPUReport::warning("[opengl] color depth mismatch, EGL: %d ~ X11 window: %d",
            attribs.egl_depth, attribs.depth);

    // Create EGL X11 Surface
    EGLSurface surface = eglCreateWindowSurface(
        device->display,
        device->config,
        (EGLNativeWindowType) win.window,
        egl_attribs_surface);
    if (surface == EGL_NO_SURFACE) {
        GPUReport::error("[opengl] failed create EGL surface");
        return nullptr;
    }

    // Create OpenGL Context
    GLContext *ctx = new GLContext();
    ctx->m_driver = this->m_driver;
    ctx->m_device = this;
    // Create OpenGL Context: X11 Context
    LinuxEGLContext *gtx = &ctx->m_egl_context;
    gtx->driver = driver;
    gtx->device = device;
    gtx->display = device->display;
    gtx->context = device->context;
    gtx->surface = surface;
    gtx->linux_is_rgba = attribs.depth == 32;
    gtx->linux_is_x11 = true;

    // Return Created Context
    m_ctx_cache.add(ctx);
    this->setVerticalSync(m_driver->m_vsync);
    GPUReport::success("[opengl] EGL X11 surface created for XID:0x%lx", win.window);
    return ctx;
}

// -------------------------------------
// Linux OpenGL Context: Wayland Context
// -------------------------------------

typedef struct wl_egl_window wl_egl_window;
typedef wl_egl_window* (*wl_egl_window_create_t) (void* surface, int w, int h);
typedef void (*wl_egl_window_resize_t) (wl_egl_window* win, int w, int h, int dx, int dy);
typedef void (*wl_egl_window_get_attached_size_t) (wl_egl_window* win, int *w, int *h);
typedef void (*wl_egl_window_destroy_t) (wl_egl_window* win);

GPUContext* GLDevice::createContextWayland(GPUWindowWayland win) {
    LinuxEGLDriver *driver = &m_driver->m_egl_driver;
    LinuxEGLDevice *device = &m_egl_device;
    if (!createContextEGL(win.display, LinuxEGLOption::LINUX_WAYLAND))
        return nullptr;

    // Open Wayland Module
    if (!driver->so_wayland) driver->so_wayland =
        dlopen("libwayland-egl.so.1", RTLD_LAZY | RTLD_GLOBAL);
    if (!driver->so_wayland) {
        GPUReport::error("[opengl] failed load libwayland-egl.so.1");
        return nullptr;
    }

    // Create Wayland EGL Window Surface
    wl_egl_window_create_t wl_egl_window_create = (wl_egl_window_create_t)
        dlsym(driver->so_wayland, "wl_egl_window_create");
    void* wl_surface = (void*) wl_egl_window_create(win.surface, win.w, win.h);
    if (wl_surface == nullptr) {
        GPUReport::error("[opengl] failed creating wayland EGL window");
        return nullptr;
    }

    // Create EGL Wayland Surface
    EGLSurface surface = eglCreateWindowSurface(
        device->display,
        device->config,
        (EGLNativeWindowType) wl_surface,
        egl_attribs_surface);
    if (surface == EGL_NO_SURFACE) {
        GPUReport::error("[opengl] failed create EGL surface");
        return nullptr;
    }

    // Create OpenGL Context
    GLContext *ctx = new GLContext();
    ctx->m_driver = this->m_driver;
    ctx->m_device = this;
    // Create OpenGL Context: Wayland Context
    LinuxEGLContext *gtx = &ctx->m_egl_context;
    gtx->driver = driver;
    gtx->device = device;
    gtx->display = device->display;
    gtx->context = device->context;
    gtx->surface = surface;
    gtx->linux_is_rgba = m_rgba;
    gtx->linux_is_x11 = false;
    // Cache Wayland EGL Proc Symbols
    gtx->wl_resize_proc = dlsym(driver->so_wayland, "wl_egl_window_resize");
    gtx->wl_dimensions_proc = dlsym(driver->so_wayland, "wl_egl_window_get_attached_size");
    gtx->wl_destroy_proc = dlsym(driver->so_wayland, "wl_egl_window_destroy");
    gtx->wl_surface = wl_surface;

    // Return Created Context
    m_ctx_cache.add(ctx);
    this->setVerticalSync(m_driver->m_vsync);
    GPUReport::success("[opengl] EGL Wayland surface created for wl_surface:%p", win.surface);
    return ctx;
}

// ------------------------------------
// Linux OpenGL Context: Object Surface
// ------------------------------------

void GLDriver::makeCurrent(GLContext* ctx) {
    if (m_ctx_current != ctx) {
        LinuxEGLContext *gtx = &ctx->m_egl_context;
        EGLDisplay display = gtx->display;
        EGLContext context = gtx->context;
        EGLSurface surface = gtx->surface;

        // Change Current Context
        eglMakeCurrent(display, surface, surface, context);
        m_ctx_current = ctx;
    }
}

void GLContext::surfaceSwap() {
    // XXX: THIS IS A DUMMY :XXX
    gl__makeCurrent();
    glClearColor(0.2, 0.2, 0.3, 0.5);
    glClear(GL_COLOR_BUFFER_BIT);
    // Swap Context Buffer
    LinuxEGLContext *gtx = &m_egl_context;
    eglSwapBuffers(gtx->display, gtx->surface);
}

void GLContext::surfaceResize(int w, int h) {
    LinuxEGLContext *gtx = &m_egl_context;
    if (gtx->linux_is_x11) return;

    // Resize Wayland Surface
    wl_egl_window_resize_t wl_egl_window_resize = 
        (wl_egl_window_resize_t) gtx->wl_resize_proc;
    wl_egl_window_resize((wl_egl_window*) gtx->wl_surface, w, h, 0, 0);
}

GPUDeviceOption GLContext::surfaceOption() {
    return m_device->m_option;
}

int GLContext::surfaceSamples() {
    return m_device->m_samples;
}

bool GLContext::surfaceRGBA() {
    return m_device->m_rgba;
}

void GLContext::destroy() {
    m_driver->makeCurrent(this);
    m_device->m_ctx_cache.remove(this);

    // Destroy EGL Surface and Remove Current
    LinuxEGLContext *gtx = &m_egl_context;
    eglMakeCurrent(gtx->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(gtx->display, gtx->surface);
    // Destroy Wayland EGL Surface
    if (gtx->linux_is_x11 == false) {
        wl_egl_window_destroy_t wl_egl_window_destroy = 
        (wl_egl_window_destroy_t) gtx->wl_destroy_proc;
        wl_egl_window_destroy((wl_egl_window*) gtx->wl_surface);
    }

    // Dealloc Object and Remove Current
    m_driver->m_ctx_current = nullptr;
    delete this;
}
