// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Cristian Camilo Ruiz <mrgaturus>
#ifndef NOGPU_MAP_H
#define NOGPU_MAP_H

unsigned int crc32c(
    unsigned int crc,
    const unsigned char *str);

class GPUHashmapOpaque {
    typedef struct {
        unsigned int key;
        unsigned int size;
        char data[0];
    } GPUHashmapItem;

    protected: // Hashmap Attributes
        GPUHashmapItem* m_buffer;
        int m_capacity, m_len;
        int m_item, m_pad;
        unsigned int find(unsigned int key);
        unsigned int crc32(const char* name);

        GPUHashmapItem* lookup(int idx);
        void insert(int idx, unsigned int key, void* data);
        void expand(int idx, unsigned int key, void* data);
        void takeoff(int idx);
    protected: // Hashmap Constructor
        GPUHashmapOpaque(int size, void* fast) noexcept;
        ~GPUHashmapOpaque() noexcept;

    protected: // Hashmap Manipulation: ID
        bool add_0(unsigned int key, void* data);
        bool remove_0(unsigned int key);
        bool check_0(unsigned int key);
        void* get_0(unsigned int key);
    protected: // Hashmap Manipulation: Name
        bool add_0(const char* name, void* data);
        bool remove_0(const char* name);
        bool check_0(const char* name);
        void* get_0(const char* name);
};

// -------------------
// GPU Hasmap Template
// -------------------

template <typename T>
class GPUHashmap : GPUHashmapOpaque {
    typedef struct {
        unsigned int key;
        unsigned int pad;
        T data;
    } GPUHashmapEntry;
    GPUHashmapEntry m_fast;
    // Hashmap Iterator
    class GPUHashmapIterator {
        unsigned char* mi_data;
        public: // Iterator Methods
            void operator++() { mi_data += sizeof(GPUHashmapEntry); }
            bool operator!=(GPUHashmapIterator a) { return mi_data != a.mi_data; }
            T operator*() { return ((GPUHashmapEntry*) mi_data)->data; }
        public: // Iterator Constructor
            static GPUHashmapIterator create(unsigned char* data, int offset) {
                const unsigned int size = sizeof(GPUHashmapEntry);
                GPUHashmapIterator i; i.mi_data = data + (size * offset);
                return i;
            }
    };

    public: // Hashmap Constructor
        GPUHashmap() noexcept : GPUHashmapOpaque(sizeof(T), &m_fast) {}
        GPUHashmapIterator begin() { return GPUHashmapIterator::create((unsigned char*) m_buffer, 0); }
        GPUHashmapIterator end() { return GPUHashmapIterator::create((unsigned char*) m_buffer, m_len); }
    public: // Hashmap Manipulation: ID
        bool add_key(unsigned int key, T data) { return add_0(key, &data); }
        bool remove_key(unsigned int key) { return remove_0(key); }
        bool check_key(unsigned int key) { return check_0(key); }
        T* get_key(unsigned int key) { return (T*) get_0(key);  }
    public: // Hashmap Manipulation: Name
        bool add_name(const char* name, T data) { return add_0(name, &data); }
        bool remove_name(const char* name) { return remove_0(name); }
        bool check_name(const char* name) { return check_0(name); }
        T* get_name(const char* name) { return (T*) get_0(name); }
};

#endif // NOGPU_MAP_H
