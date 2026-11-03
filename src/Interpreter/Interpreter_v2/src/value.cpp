//
// Created by YangKai.Lin on 2026/11/3.
//

#include "value.h"
#include "chunk_memory.h"
#include <cstdio>

namespace CLOX_V2::Const {
    void initValueArray(ValueArray *array) {
        array->values = nullptr;
        array->capacity = 0;
        array->count = 0;
    }

    void writeValueArray(ValueArray *array, Value value) {
        if (array->capacity < array->count + 1) {
            int oldCapacity = array->capacity;
            array->capacity = GROW_CAPACITY(oldCapacity);
            array->values = GROW_ARRAY(Value, array->values, oldCapacity, array->capacity);
        }
        array->values[array->count] = value;
        array->count++;
    }

    void freeValueArray(ValueArray *array) {
        FREE_ARRAY(Value, array->values, array->capacity);
        initValueArray(array);
    }

    void printValue(Const::Value value) {
        printf("%g", value);
    }
}
