//
// Created by YangKai.Lin on 2026/11/3.
//

#ifndef TOYPROJECTS_Interpreter_v2_VALUE_H
#define TOYPROJECTS_Interpreter_v2_VALUE_H

#include "common.h"

namespace CLOX_V2::Const {
    typedef struct Obj Obj;
    typedef struct ObjString ObjString;

    typedef enum {
        VAL_BOOL,
        VAL_NIL,
        VAL_NUMBER,
        VAL_OBJ
      } ValueType;

    typedef struct {
        ValueType type;
        union {
            bool boolean;
            double number;
            Obj* obj;
        } as;
    } Value;

    inline Value makeBool(bool value) {
        return {VAL_BOOL, {.boolean = value}};
    }

    inline Value makeNil() {
        return {VAL_NIL, {.number = 0}};
    }

    inline Value makeNumber(double value) {
        return {VAL_NUMBER, {.number = value}};
    }

    inline Value makeObj(Obj* value) {
        return {VAL_OBJ, {.obj = value}};
    }

    inline bool isBool(const Value &v) { return v.type == VAL_BOOL; }
    inline bool isNil(const Value &v) { return v.type == VAL_NIL; }
    inline bool isNumber(const Value &v) { return v.type == VAL_NUMBER; }
    inline bool isObj(const Value &v) { return v.type == VAL_OBJ; }
    inline bool asObj(const Value &v) { return v.as.obj;}
    inline bool asBool(const Value &v) { return v.as.boolean; }
    inline double asNumber(const Value &v) { return v.as.number; }
}

#define IS_BOOL(v)    (CLOX_V2::Const::isBool(v))
#define IS_NIL(v)     (CLOX_V2::Const::isNil(v))
#define IS_NUMBER(v)  (CLOX_V2::Const::isNumber(v))
#define IS_OBJ(v)     (CLOX_V2::Const::isObj(v))
#define AS_OBJ(v)     (CLOX_V2::Const::asObj(v))
#define AS_BOOL(v)    (CLOX_V2::Const::asBool(v))
#define AS_NUMBER(v)  (CLOX_V2::Const::asNumber(v))
#define BOOL_VAL(v)   (CLOX_V2::Const::makeBool(v))
#define NIL_VAL       (CLOX_V2::Const::makeNil())
#define NUMBER_VAL(v) (CLOX_V2::Const::makeNumber(v))
#define OBJ_VAL(v)    (CLOX_V2::Const::makeObj(v))

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
