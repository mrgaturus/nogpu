// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "nogpu_private.h"
#include <cstdarg>
#include <cstdio>
// Stacktrace Debug
#ifdef NOGPU_DEBUG
#include <cpptrace/cpptrace.hpp>
#endif

typedef enum {
    LOG_INFO,
    LOG_SUCCESS,
    LOG_WARNING,
    LOG_ERROR,
    LOG_ASSERT,
    LOG_DEBUG,
} GPUReportLevel;

const char* log_headers[] = {
    "\e[0;34m[nogpu: info]\033[0m",
    "\e[0;32m[nogpu: ok]\033[0m",
    "\e[0;33m[nogpu: warning]\033[0m",
    "\e[0;31m[nogpu: error]\033[0m",
    "\e[0;31m[nogpu: assert]\033[0m",
    "\e[0;37m[nogpu: debug]\033[0m",
};

static void inline printf_level(GPUReportLevel level, const char *format, va_list args) {
    printf("%s ", log_headers[level]);
    vprintf(format, args);
    printf("\n");
}

// -----------------
// Basic GPU Logging
// -----------------

void GPUReport::info(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf_level(LOG_INFO, format, args);
    va_end(args);
}

void GPUReport::success(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf_level(LOG_SUCCESS, format, args);
    va_end(args);
}

void GPUReport::warning(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf_level(LOG_WARNING, format, args);
    va_end(args);
}

void GPUReport::error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf_level(LOG_ERROR, format, args);
    va_end(args);
}

// -----------------
// Debug GPU Logging
// -----------------

void GPUReport::assert(bool condition, const char *format, ...) {
#ifdef NOGPU_DEBUG
    if (condition)
        return;

    // Print Error Message
    va_list args;
    va_start(args, format);
    printf_level(LOG_ASSERT, format, args);
    va_end(args);

    // Exit Program
    cpptrace::generate_trace();
    exit(~0);
#endif
}

void GPUReport::debug(const char *format, ...) {
#ifdef NOGPU_DEBUG
    va_list args;
    va_start(args, format);
    printf_level(LOG_DEBUG, format, args);
    va_end(args);
#endif
}

void GPUReport::stacktrace() {
#ifdef NOGPU_DEBUG
    cpptrace::generate_trace().print();
#endif
}
