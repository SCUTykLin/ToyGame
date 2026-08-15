//
// Created by 15813 on 2026/8/15.
//

#ifndef TOYPROJECTS_CLOX_MEMORY_H
#define TOYPROJECTS_CLOX_MEMORY_H

#include "common.h"

namespace CLOX::Mem {
    void *reallocate(void *pointer, size_t oldSize, size_t newSize);
}

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)CLOX::Mem::reallocate(pointer, sizeof(type) * (oldCount), \
        sizeof(type) * (newCount))

#define FREE_ARRAY(type, pointer, oldCount) \
    CLOX::Mem::reallocate(pointer, sizeof(type) * (oldCount), 0)

#endif //TOYPROJECTS_CLOX_MEMORY_H
