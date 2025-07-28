// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_PRIVATE_H
#define NOGPU_PRIVATE_H
#include <nogpu/device.h>
#include <nogpu/texture.h>

unsigned int next_power_of_two(unsigned int v);
unsigned int levels_power_of_two(int w, int h, int limit);
unsigned int device_driver_flag(GPUDeviceDriver driver);
unsigned int device_feature_flag(GPUDeviceFeature feature);
// Texture Pixel Types: Operations
bool canTextureBuffer(GPUTexturePixelType type);
bool canTransferChange(GPUTexturePixelType type);
GPUTextureTransferFormat computeTransferFormat(GPUTexturePixelType type);
GPUTextureTransferSize computeTransferSize(GPUTexturePixelType type);
int computeBytesPerPixel(GPUTextureTransferFormat format, GPUTextureTransferSize size);

// ------------------
// GPU Simple Hashmap
// ------------------

template <typename T> class GPUHashmap {
    typedef struct {
        unsigned int hash;
        unsigned int pad;
        T data;
    } GPUHashmapEntry;
    // Hashmap Buffer
    typedef union {
        GPUHashmapEntry* data_list;
        GPUHashmapEntry data_fast;
    } GPUHashmapData;

    private: // Hashmap Attributes
        int m_len, m_capacity;
        GPUHashmapData m_data;
        unsigned int find(unsigned int hash);
        unsigned int crc32(char* name);

    public: // Hashmap Constructor
        GPUHashmap() noexcept;
        ~GPUHashmap() noexcept;
    public: // Hashmap Manipulation: ID
        bool add(unsigned int hash, T &data);
        bool check(unsigned int hash);
        bool remove(unsigned int hash);
        T* get(unsigned int hash);
    public: // Hashmap Manipulation: Name
        bool add(char* name, T &data);
        bool check(char* name);
        bool remove(char* name);
        T* get(char* name);
};

// ------------------
// GPU Logger Console
// ------------------

class GPUReport {
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
