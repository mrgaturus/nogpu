// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#include "nogpu_private.h"
#include <cstdlib>
#include <cstring>

// -------------------
// Hashmap Constructor
// -------------------

template <typename T>
GPUHashmap<T>::GPUHashmap() noexcept {
    m_len = 0;
    m_capacity = 0;
    m_data = (GPUHashmapData) {};
}

template <typename T>
GPUHashmap<T>::~GPUHashmap() noexcept {
    if (m_capacity > 1)
        free(m_data.data_list);
}

// ------------------
// Hashmap Attributes
// ------------------

template <typename T>
unsigned int GPUHashmap<T>::find(unsigned int hash) {

}

template <typename T>
unsigned int GPUHashmap<T>::crc32(char* name) {

}

// ------------------------
// Hashmap Manipulation: ID
// ------------------------

template <typename T>
bool GPUHashmap<T>::add(unsigned int hash, T &data) {

}

template <typename T>
bool GPUHashmap<T>::check(unsigned int hash) {

}

template <typename T>
bool GPUHashmap<T>::remove(unsigned int hash) {

}

template <typename T>
T* GPUHashmap<T>::get(unsigned int hash) {

}

// --------------------------
// Hashmap Manipulation: Name
// --------------------------

template <typename T>
bool GPUHashmap<T>::add(char* hash, T &data) {

}

template <typename T>
bool GPUHashmap<T>::check(char* hash) {

}

template <typename T>
bool GPUHashmap<T>::remove(char* hash) {

}

template <typename T>
T* GPUHashmap<T>::get(char* hash) {

}
