// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "nogpu_map.h"
#include <cstdlib>
#include <cstring>

// -------------------
// Hashmap Constructor
// -------------------

GPUHashmapOpaque::GPUHashmapOpaque(int size, void* fast) noexcept {
    m_buffer = (GPUHashmapItem*) fast;
    m_item = size;
    m_capacity = 1;
    m_len = 0;
}

GPUHashmapOpaque::~GPUHashmapOpaque() noexcept {
    if (m_capacity > 1)
        free(m_buffer);
}

// ---------------------
// Hashmap Data: Finding
// ---------------------

unsigned int GPUHashmapOpaque::find(unsigned int key) {
    unsigned char* buffer = (unsigned char*) m_buffer;
    const unsigned int head = sizeof(GPUHashmapItem);
    const unsigned int item = m_item;
    int stride = head + item;

    unsigned int idx = 0;
    unsigned int end = m_len;
    unsigned int mid;
    // Find Upper Bound
    while (idx < end) {
        mid = idx + ((end - idx) >> 1);
        GPUHashmapItem* item = (GPUHashmapItem*) (buffer + mid * stride);
        // Check Item Upper Bound
        if (item->key < key)
            idx = mid + 1;
        else end = mid;
    }

    // Return Found
    return idx;
}

unsigned int GPUHashmapOpaque::crc32(const char* name) {
    unsigned int seed = reinterpret_cast<unsigned long long>(this);
    return crc32c(seed, (unsigned char*) name);
}

// --------------------
// Hashmap Data: Buffer
// --------------------

GPUHashmapOpaque::GPUHashmapItem* GPUHashmapOpaque::lookup(int idx) {
    unsigned char* buffer = (unsigned char*) m_buffer;
    const unsigned int head = sizeof(GPUHashmapItem);
    const unsigned int item = m_item;

    // Lookup Hashmap Item
    idx *= head + item;
    return (GPUHashmapItem*) (buffer + idx);
}

void GPUHashmapOpaque::insert(int idx, unsigned int key, void* data) {
    unsigned char* buffer = (unsigned char*) m_buffer;
    const unsigned int head = sizeof(GPUHashmapItem);
    const unsigned int item = m_item;
    // Scale Index to Stride Bytes
    int stride = head + item;
    int len = m_len * stride;
    idx *= stride;

    // Shift Elements to Right
    if (m_capacity > 1)
        memmove(buffer + idx + stride, buffer + idx, len - idx);
    // Put Element at Position
    memcpy(buffer + idx, &key, sizeof(unsigned int));
    memcpy(buffer + idx + head, data, item);
    m_len++;
}

void GPUHashmapOpaque::expand(int idx, unsigned int key, void* data) {
    const unsigned int head = sizeof(GPUHashmapItem);
    const unsigned int item = m_item;
    // Scale Index to Stride Bytes
    int capacity = m_capacity * 2;
    int stride = head + item;
    int len = m_len * stride;
    idx *= stride;

    // Create Buffer With Twice Capacity
    unsigned char* buffer0 = (unsigned char*) m_buffer;
    unsigned char* buffer = (unsigned char*) malloc(stride * capacity);    
    // Copy Buffer Data to New Buffer
    memcpy(buffer, buffer0, idx);
    memcpy(buffer + idx + stride, buffer0 + idx, len - idx);
    memcpy(buffer + idx, &key, sizeof(unsigned int));
    memcpy(buffer + idx + head, data, item);

    // Change Current Buffer
    if (capacity > 2)
        free(buffer0);
    m_buffer = (GPUHashmapItem*) buffer;
    m_capacity = capacity;
    m_len++;
}

void GPUHashmapOpaque::takeoff(int idx) {
    unsigned char* buffer = (unsigned char*) m_buffer;
    const unsigned int head = sizeof(GPUHashmapItem);
    const unsigned int item = m_item;
    // Scale Index to Stride Bytes
    int stride = head + item;
    int len = m_len * stride;
    idx *= stride;

    // Shift Elements to Right
    memcpy(buffer + idx, buffer + idx + stride,
        len - idx - stride);
    // Remove Item
    m_len--;
}

// ------------------------
// Hashmap Manipulation: ID
// ------------------------

bool GPUHashmapOpaque::add_key0(unsigned int key, void* data) {
    int idx = this->find(key);
    bool check = idx >= m_len || this->lookup(idx)->key != key;
    // Check and Add to Hashmap
    if (check) {
        if (m_len < m_capacity)
            this->insert(idx, key, data);
        else this->expand(idx, key, data);
    }

    // Return Success
    return check;
}

bool GPUHashmapOpaque::replace_key0(unsigned int key, void* data) {
    int idx = this->find(key);
    bool check = idx >= m_len || this->lookup(idx)->key != key;

    // Add or Replace to Hashmap
    if (check) {
        if (m_len < m_capacity)
            this->insert(idx, key, data);
        else this->expand(idx, key, data);
    } else {
        void* target = this->lookup(idx)->data;
        memcpy(target, data, m_item);
    }

    // Return Was Replaced
    return !check;
}

bool GPUHashmapOpaque::remove_key0(unsigned int key) {
    int idx = this->find(key);
    // Check and Remove from Hashmap
    bool check = idx < m_len && this->lookup(idx)->key == key;
    if (check) this->takeoff(idx);

    // Return Success
    return check;
}

bool GPUHashmapOpaque::check_key0(unsigned int key) {
    int idx = this->find(key);
    bool check = idx < m_len;
    if (!check) return check;

    // Check if Item is Found
    GPUHashmapItem* item = this->lookup(idx);
    return item->key == key;
}

void* GPUHashmapOpaque::get_key0(unsigned int key) {
    int idx = this->find(key);
    bool check = idx < m_len;

    void* data = nullptr;
    if (!check) return data;
    // Check if Item is Found
    GPUHashmapItem* item = this->lookup(idx);
    if (item->key == key)
        data = &item->data;
    // Return Found Item
    return data;
}

// --------------------------
// Hashmap Manipulation: Name
// --------------------------

bool GPUHashmapOpaque::add_name0(const char* hash, void* data) {
    unsigned int crc32 = this->crc32(hash);
    return this->add_key0(crc32, data);
}

bool GPUHashmapOpaque::replace_name0(const char* hash, void* data) {
    unsigned int crc32 = this->crc32(hash);
    return this->replace_key0(crc32, data);
}

bool GPUHashmapOpaque::remove_name0(const char* hash) {
    unsigned int crc32 = this->crc32(hash);
    return this->remove_key0(crc32);
}

bool GPUHashmapOpaque::check_name0(const char* hash) {
    unsigned int crc32 = this->crc32(hash);
    return this->check_key0(crc32);
}

void* GPUHashmapOpaque::get_name0(const char* hash) {
    unsigned int crc32 = this->crc32(hash);
    return this->get_key0(crc32);
}
