//
// Created by 15813 on 2026/8/15.
//

#ifndef TOYPROJECTS_CLOX_VM_H
#define TOYPROJECTS_CLOX_VM_H

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

namespace CLOX::Vm {
    typedef struct {
        Chunk::Chunk *chunk;
        uint8_t* ip;
        Value::Value stack[STACK_MAX];
        Value::Value* stackTop;
    } VM;

    typedef enum {
        INTERPRET_OK,
        INTERPRET_COMPILE_ERROR,
        INTERPRET_RUNTIME_ERROR
    } InterpretResult;

    void initVM();

    void freeVM();

    InterpretResult interpret(const char* source);

    void push(Value::Value value);
    Value::Value pop();
}

#endif //TOYPROJECTS_CLOX_VM_H
