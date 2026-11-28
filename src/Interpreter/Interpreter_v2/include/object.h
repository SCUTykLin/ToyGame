//
// Created by 15813 on 2026-11-28.
//

#ifndef TOYPROJECTS_Interpreter_v2_OBJECT_H
#define TOYPROJECTS_Interpreter_v2_OBJECT_H

#include "common.h"
#include "value.h"

#define OBJ_TYPE(value) (AS_OBJ(value)->type)
#define IS_STRING(value) CLOX_V2::Const::isObjType(value, CLOX_V2::Const::OBJ_STRING);
#define AS_STRING(value) ((CLOX_V2::Const::ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((CLOX_V2::Const::ObjString*)AS_OBJ(value))->chars)

namespace CLOX_V2::Const {
    typedef enum {
        OBJ_STRING,
    } ObjType;

    struct Obj {
        ObjType type;
    };

    struct ObjString {
        Obj obj;
        int length;
        char *chars;
    };

    static inline bool isObjType(Value value, ObjType type) {
        return IS_OBJ(value) && AS_OBJ(value)->type == type;
    }
}

#endif //TOYPROJECTS_Interpreter_v2_OBJECT_H
