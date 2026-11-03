//
// Created by YangKai.Lin on 2026/11/3.
//

#ifndef TOYPROJECTS_Interpreter_v2_VM_H
#define TOYPROJECTS_Interpreter_v2_VM_H

#include "chunk.h"
#define STACK_MAX 256

namespace CLOX_V2::VM {
    typedef struct {
        Chunk::Chunk *chunk;
        uint8_t* ip; // 指令指针
        Const::Value stack[STACK_MAX];
        Const::Value* stackTop;
    } VM;

    typedef enum {
        INTERPRET_OK,
        INTERPRET_COMPILE_ERROR,
        INTERPRET_RUNTIME_ERROR
    } InterpretResult;

    void initVM();

    InterpretResult interpret(const char* source);

    void push(Const::Value value);
    Const::Value pop();

    void freeVM();
}

#endif //TOYPROJECTS_Interpreter_v2_VM_H
