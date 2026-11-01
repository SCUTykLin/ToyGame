//
// Created by 15813 on 2026-11-02.
//
#include <cstdlib>
#include "chunk_memory.h"

namespace CLOX_V2::Mem {
    void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
        if (newSize == 0) {
            free(pointer);
            return nullptr;
        }
        void* result = realloc(pointer, newSize);
        if (result == nullptr) exit(1);
        return result;
    }
}

