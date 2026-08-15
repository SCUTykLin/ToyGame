//
// Created by 15813 on 2026/8/15.
//
#include "clox_memory.h"
#include <stdlib.h>

namespace CLOX::Mem {
    void *reallocate(void *pointer, size_t oldSize, size_t newSize) {
        if (newSize == 0) {
            free(pointer);
            return nullptr;
        }

        void *result = realloc(pointer, newSize);
        if (result == NULL) exit(1);
        return result;
    }
}