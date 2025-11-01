// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_PRIVATE_H
#define NOGPU_PRIVATE_H
#include <nogpu/device.h>

class GPUContextCache {
    public: // Context Cache
        GPUContext* m_list;
        GPUContext* find(void* window);
        void add(GPUContext* ctx);
        void remove(GPUContext* ctx);
};

unsigned int next_power_of_two(unsigned int v);
unsigned int levels_power_of_two(int w, int h, int limit);
unsigned int driver_option_flag(GPUDriverOption driver);
unsigned int driver_feature_flag(GPUDriverFeature feature);

enum class GPUUniformType;
int computeUniformBytes(GPUUniformType type);
// Texture Pixel Types: Operations
enum class GPUTexturePixelType;
enum class GPUTextureTransferFormat;
enum class GPUTextureTransferSize;
bool canTextureBuffer(GPUTexturePixelType type);
bool canTransferChange(GPUTexturePixelType type);
GPUTextureTransferFormat computeTransferFormat(GPUTexturePixelType type);
GPUTextureTransferSize computeTransferSize(GPUTexturePixelType type);
int computeTransferBytesPerPixel(GPUTextureTransferFormat format, GPUTextureTransferSize size);

// ---------------------
// GPU Driver: Reporting
// ---------------------

class GPUReport {
    friend GPUDriver;
    friend GPUDevice;
    // Report Attributes
    static GPUReport m_report;
    GPUDebugCallback m_callback;
    GPUDriverMode m_mode;
    void* m_userdata;
    void* m_object;

    public: // Reporting Properties
        static void setMessage(GPUDebugLevel level, const char* message, int size);
        static void setCallback(GPUDebugCallback cb, void* userdata);
        static void setMode(GPUDriverMode mode);
        static void setObject(void* object);
        static bool getEnabled();

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

            static void assert(bool condition, const char* format, ...)
                __attribute__ ((format (printf, 2, 3)));
            static void debug(const char* format, ...)
                __attribute__ ((format (printf, 1, 2)));
    #else
        public:
            static void info(const char* format, ...);
            static void success(const char* format, ...);
            static void warning(const char* format, ...);
            static void error(const char* format, ...);

            static void assert(bool condition, const char* format, ...);
            static void debug(const char* format, ...);
    #endif
};

#endif // NOGPU_PRIVATE_H
