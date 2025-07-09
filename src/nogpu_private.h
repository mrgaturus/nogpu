// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_PRIVATE_H
#define NOGPU_PRIVATE_H
#include <nogpu/driver.h>
#include <nogpu/texture.h>

unsigned int next_power_of_two(unsigned int v);
unsigned int feature_flag(GPUDriverFeature feature);
// Texture Pixel Types: Operations
bool canTextureBuffer(GPUTexturePixelType type);
bool canTransferChange(GPUTexturePixelType type);
GPUTextureTransferFormat computeTransferFormat(GPUTexturePixelType type);
GPUTextureTransferSize computeTransferSize(GPUTexturePixelType type);
int computeBytesPerPixel(GPUTextureTransferFormat format, GPUTextureTransferSize size);

// ------------------
// GPU Logger Console
// ------------------

class GPULogger {
    #if defined(__clang__) || defined(__GNUC__)
        public:
            static void info(const char* format, ...)
                __attribute__ ((format (printf, 1, 2)));
            static void success(const char* format, ...)
                __attribute__ ((format (printf, 1, 2)));
            static void warning(const char* format, ...)
                __attribute__ ((format (printf, 1, 2)));
            static void error(const char* format, ...)
                __attribute__ ((format (printf, 1, 2)));

            // Stacktrace Debugging
            static void assert(bool condition, const char* format, ...)
                __attribute__ ((format (printf, 2, 3)));
            static void debug(const char* format, ...)
                __attribute__ ((format (printf, 1, 2)));
            static void stacktrace();
    #else
        public:
            static void info(const char* format, ...);
            static void success(const char* format, ...);
            static void warning(const char* format, ...);
            static void error(const char* format, ...);

            // Stacktrace Debugging
            static void assert(bool condition, const char* format, ...);
            static void debug(const char* format, ...);
            static void stacktrace();
    #endif
};

#endif // NOGPU_PRIVATE_H
