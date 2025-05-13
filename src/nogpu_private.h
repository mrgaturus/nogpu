// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_PRIVATE_H
#define NOGPU_PRIVATE_H

class GPULogger {
    #if defined(__clang__) || defined(__GNUC__)
        public:
            static void info(char* format, ...)
                __attribute__ ((format (printf, 1, 2)));
            static void success(char* format, ...)
                __attribute__ ((format (printf, 1, 2)));
            static void warning(char* format, ...)
                __attribute__ ((format (printf, 1, 2)));
            static void error(char* format, ...)
                __attribute__ ((format (printf, 1, 2)));

            // Stacktrace Debugging
            static void debug(char* format, ...)
                __attribute__ ((format (printf, 1, 2)));
            static void debug_stacktrace(char* format, ...)
                __attribute__ ((format (printf, 1, 2)));
    #else
        public:
            static void info(char* format, ...);
            static void success(char* format, ...);
            static void warning(char* format, ...);
            static void error(char* format, ...);

            // Stacktrace Debugging
            static void debug(char* format, ...);
            static void debug_stacktrace(char* format, ...);
    #endif
};

#endif // NOGPU_PRIVATE_H
