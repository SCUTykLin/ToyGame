//
// Created by YangKai.Lin on 2026/11/3.
//

#ifndef TOYPROJECTS_Interpreter_v2_VALUE_H
#define TOYPROJECTS_Interpreter_v2_VALUE_H

#include "common.h"

namespace CLOX_V2::Const {
    typedef enum {
        VAL_BOOL,
        VAL_NIL,
        VAL_NUMBER,
      } ValueType;

    typedef struct {
        ValueType type;
        union {
            bool boolean;
            double number;
        } as;
    } Value;

    inline Value makeBool(bool value) {
        Value result{};
        result.type = VAL_BOOL;
        result.as.boolean = value;
        return result;
    }

    inline Value makeNil() {
        Value result{};
        result.type = VAL_NIL;
        result.as.number = 0;
        return result;
    }

    inline Value makeNumber(double value) {
        Value result{};
        result.type = VAL_NUMBER;
        result.as.number = value;
        return result;
    }

    inline bool isBool(const Value &v) { return v.type == VAL_BOOL; }
    inline bool isNil(const Value &v) { return v.type == VAL_NIL; }
    inline bool isNumber(const Value &v) { return v.type == VAL_NUMBER; }
    inline bool asBool(const Value &v) { return v.as.boolean; }
    inline double asNumber(const Value &v) { return v.as.number; }
}

#define IS_BOOL(v)    (CLOX_V2::Const::isBool(v))
#define IS_NIL(v)     (CLOX_V2::Const::isNil(v))
#define IS_NUMBER(v)  (CLOX_V2::Const::isNumber(v))
#define AS_BOOL(v)    (CLOX_V2::Const::asBool(v))
#define AS_NUMBER(v)  (CLOX_V2::Const::asNumber(v))
#define BOOL_VAL(v)   (CLOX_V2::Const::makeBool(v))
#define NIL_VAL       (CLOX_V2::Const::makeNil())
#define NUMBER_VAL(v) (CLOX_V2::Const::makeNumber(v))

namespace CLOX_V2::Const {

    typedef struct {
        int capacity;
        int count;
        Value *values;
    } ValueArray;

    bool valuesEqual(Value a, Value b);

    void initValueArray(ValueArray *array);

    void writeValueArray(ValueArray *array, Value value);

    void freeValueArray(ValueArray *array);

    void printValue(Const::Value value);
}
#endif //TOYPROJECTS_Interpreter_v2_VALUE_H
