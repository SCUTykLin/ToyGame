//
// Created by YangKai.Lin on 2026/11/3.
//

#ifndef TOYPROJECTS_Interpreter_v2_VALUE_H
#define TOYPROJECTS_Interpreter_v2_VALUE_H

#include "common.h"

namespace CLOX_V2::Const {
    typedef double Value;

    typedef struct {
        int capacity;
        int count;
        Value *values;
    } ValueArray;

    void initValueArray(ValueArray *array);

    void writeValueArray(ValueArray *array, Value value);

    void freeValueArray(ValueArray *array);

    void printValue(Const::Value value);
}
#endif //TOYPROJECTS_Interpreter_v2_VALUE_H
