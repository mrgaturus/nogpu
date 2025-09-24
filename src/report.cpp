// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "nogpu_private.h"
#include <csignal>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>

GPUReport GPUReport::m_report = {};
const char* debug_headers[] = {
    "\e[0;34m[nogpu: info]\033[0m",
    "\e[0;32m[nogpu: ok]\033[0m",
    "\e[0;33m[nogpu: warning]\033[0m",
    "\e[0;31m[nogpu: error]\033[0m",
    "\e[0;31m[nogpu: assert]\033[0m",
    "\e[0;37m[nogpu: log]\033[0m",
};

// ----------------
// GPU Report State
// ----------------

void GPUReport::setMessage(GPUDebugLevel level, const char* message, int size) {
    // Dispatch Message Reporting
    switch (m_report.m_mode) {
        case GPUDriverMode::DRIVER_MODE_DEBUG: {
            const int lvl = static_cast<int>(level);
            const char* head = debug_headers[lvl];
            // Output Debug Message
            if (m_report.m_object != nullptr)
                printf("%s [%p] %s\n", head, m_report.m_object, message);
            else printf("%s %s\n", head, message);

            // Issue Debug Breakpoint
            switch (level) {
                default: break;
                case GPUDebugLevel::DEBUG_ERROR:
                    #ifdef __unix__
                        raise(SIGTRAP);
                    #endif
                // Avoid Pass Debugging Stuff
                case GPUDebugLevel::DEBUG_ASSERT:
                case GPUDebugLevel::DEBUG_LOG:
                    return;
            }
        }

        case GPUDriverMode::DRIVER_MODE_REPORT:
            if (m_report.m_callback != nullptr) {
                GPUDebugReport re{};
                // Prepare Callback Message
                re.object = m_report.m_object;
                re.message = message;
                re.message_size = size;
                re.level = level;
                // Dispatch Callback Reporting
                m_report.m_callback(m_report.m_userdata, re);
            }

        // Debug not Enabled
        default: break;
    }
}

static void setMessageFormat(GPUDebugLevel level, const char *format, va_list args) {
    if (GPUReport::getEnabled() == false)
        return;

    va_list tmp;
    va_copy(tmp, args);
    static char buffer[128];
    char* message = buffer;
    // Output Message to Temporal Buffer
    int len = vsnprintf(message, 128, format, tmp);
    va_end(tmp);

    // Check Extra Buffer
    if (len + 1 > 128) {
        message = (char*) malloc(len + 1);
        len = vsnprintf(message, len + 1, format, args);
    }

    // Dispatch Message
    message[len] = '\0';
    GPUReport::setMessage(level, message, len);
    if (len + 1 > 128)
        free(message);
}

void GPUReport::setCallback(GPUDebugCallback cb, void* userdata) {
    m_report.m_callback = cb;
    m_report.m_userdata = userdata;    
}

void GPUReport::setMode(GPUDriverMode mode) {
    m_report.m_mode = mode;
}

void GPUReport::setObject(void* object) {
    m_report.m_object = object;
}

bool GPUReport::getEnabled() {
    switch (m_report.m_mode) {
        case GPUDriverMode::DRIVER_MODE_REPORT:
        case GPUDriverMode::DRIVER_MODE_DEBUG:
            return true;
        default: return false;
    }
}

// -------------------
// GPU Report Messages
// -------------------

void GPUReport::info(const char *format, ...) {
    va_list args;
    va_start(args, format);
    setMessageFormat(GPUDebugLevel::DEBUG_INFO, format, args);
    va_end(args);
}

void GPUReport::success(const char *format, ...) {
    va_list args;
    va_start(args, format);
    setMessageFormat(GPUDebugLevel::DEBUG_SUCCESS, format, args);
    va_end(args);
}

void GPUReport::warning(const char *format, ...) {
    va_list args;
    va_start(args, format);
    setMessageFormat(GPUDebugLevel::DEBUG_WARNING, format, args);
    va_end(args);
}

void GPUReport::error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    setMessageFormat(GPUDebugLevel::DEBUG_ERROR, format, args);
    va_end(args);
}

// --------------------------
// GPU Report Messages: Debug
// --------------------------

void GPUReport::assert(bool condition, const char *format, ...) {
    if (condition && m_report.m_mode != GPUDriverMode::DRIVER_MODE_DEBUG)
        return;

    // Print Error Message
    va_list args;
    va_start(args, format);
    setMessageFormat(GPUDebugLevel::DEBUG_ASSERT, format, args);
    va_end(args);
    // Exit Program
    exit(~0);
}

void GPUReport::debug(const char *format, ...) {
    if (m_report.m_mode != GPUDriverMode::DRIVER_MODE_DEBUG)
        return;

    va_list args;
    va_start(args, format);
    setMessageFormat(GPUDebugLevel::DEBUG_LOG, format, args);
    va_end(args);
}
