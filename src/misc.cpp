// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include <nogpu_private.h>
#include <nogpu.h>

unsigned int next_power_of_two(unsigned int v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    // Portable for all C Compilers
    return v;
}

// -----------------------
// Enum Bit-flags Operator
// -----------------------

GPUBufferMapping operator|(GPUBufferMapping a, GPUBufferMapping b) {
    return (GPUBufferMapping) ((int) a | (int) b);
}

GPUBufferMapping operator&(GPUBufferMapping a, GPUBufferMapping b) {
    return (GPUBufferMapping) ((int) a & (int) b);
}

GPUMemoryBarrier operator|(GPUMemoryBarrier a, GPUMemoryBarrier b) {
    return (GPUMemoryBarrier) ((int) a | (int) b);
}

GPUMemoryBarrier operator&(GPUMemoryBarrier a, GPUMemoryBarrier b) {
    return (GPUMemoryBarrier) ((int) a & (int) b);
}
