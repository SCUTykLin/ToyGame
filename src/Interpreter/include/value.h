//
// Created by 15813 on 2026/8/15.
//

#ifndef TOYPROJECTS_CLOX_VALUE_H
#define TOYPROJECTS_CLOX_VALUE_H

#include "common.h"

namespace CLOX::Value {
    typedef double Value;

    typedef struct {
        int capacity;
        int count;
        Value* values;
    } ValueArray;

    void initValueArray(ValueArray* array);
    void writeValueArray(ValueArray* array, Value value);
    void freeValueArray(ValueArray* array);
    void printValue(Value value);
}

#endif //TOYPROJECTS_CLOX_VALUE_H
