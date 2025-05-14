// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_PRIVATE_H
#define NOGPU_PRIVATE_H

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
            static void debug(const char* format, ...)
                __attribute__ ((format (printf, 1, 2)));
            static void debug_stacktrace(const char* format, ...)
                __attribute__ ((format (printf, 1, 2)));
    #else
        public:
            static void info(const char* format, ...);
            static void success(const char* format, ...);
            static void warning(const char* format, ...);
            static void error(const char* format, ...);

            // Stacktrace Debugging
            static void debug(const char* format, ...);
            static void debug_stacktrace(const char* format, ...);
    #endif
};

#endif // NOGPU_PRIVATE_H
