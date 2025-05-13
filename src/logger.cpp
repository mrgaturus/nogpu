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
    LOG_DEBUG,
} GPULoggerLevel;

const char* log_headers[] = {
    "\e[0;34m[nogpu: info]\033[0m",
    "\e[0;32m[nogpu: ok]\033[0m",
    "\e[0;33m[nogpu: warning]\033[0m",
    "\e[0;31m[nogpu: error]\033[0m",
    "\e[0;37m[nogpu: debug]\033[0m",
};

static void inline printf_level(GPULoggerLevel level, bool stacktrace, char *format, va_list args) {
    printf("%s ", log_headers[level]);
    vprintf(format, args);
    puts("\n");

    // Print Stacktrace
    #ifdef NOGPU_DEBUG
    if (stacktrace)
        cpptrace::generate_trace().print();
    #endif
}

// -----------------
// Basic GPU Logging
// -----------------

void GPULogger::info(char *format, ...) {
    va_list args;
    va_start(args, format);
    printf_level(LOG_INFO, false, format, args);
    va_end(args);
}

void GPULogger::success(char *format, ...) {
    va_list args;
    va_start(args, format);
    printf_level(LOG_SUCCESS, false, format, args);
    va_end(args);
}

void GPULogger::warning(char *format, ...) {
    va_list args;
    va_start(args, format);
    printf_level(LOG_WARNING, false, format, args);
    va_end(args);
}

void GPULogger::error(char *format, ...) {
    va_list args;
    va_start(args, format);
    printf_level(LOG_ERROR, true, format, args);
    va_end(args);
}

// -----------------
// Debug GPU Logging
// -----------------


void GPULogger::debug(char *format, ...) {
#ifdef NOGPU_DEBUG
    va_list args;
    va_start(args, format);
    printf_level(LOG_DEBUG, false, format, args);
    va_end(args);
#endif
}

void GPULogger::debug_stacktrace(char *format, ...) {
#ifdef NOGPU_DEBUG
    va_list args;
    va_start(args, format);
    printf_level(LOG_DEBUG, true, format, args);
    va_end(args);
#endif
}
