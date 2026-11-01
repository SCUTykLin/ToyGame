//
// Created by 15813 on 2026-11-02.
//

#ifndef TOYPROJECTS_Interpreter_v2_CHUNK_MEMORY_H
#define TOYPROJECTS_Interpreter_v2_CHUNK_MEMORY_H
#include "common.h"

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)CLOX_V2::Mem::reallocate(pointer, sizeof(type) * (oldCount), \
        sizeof(type) * (newCount))

#define FREE_ARRAY(type, pointer, oldCount) \
    CLOX_V2::Mem::reallocate(pointer, sizeof(type) * (oldCount), 0)

namespace CLOX_V2::Mem {
    void* reallocate(void* pointer, size_t oldSize, size_t newSize);
}

#endif //TOYPROJECTS_Interpreter_v2_CHUNK_MEMORY_H
