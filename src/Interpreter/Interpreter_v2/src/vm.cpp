//
// Created by YangKai.Lin on 2026/11/3.
//
#include "common.h"
#include "vm.h"
#include "debug.h"
#include <cstdio>
#include "compiler.h"

namespace CLOX_V2::VM {
    VM vm;

    static void resetStack() {
        vm.stackTop = vm.stack;
    }

    void initVM() {
        resetStack();
    }

    void push(Const::Value value) {
        *vm.stackTop = value;
        vm.stackTop++;
    }

    Const::Value pop() {
        vm.stackTop--;
        return *vm.stackTop;
    }

    static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
    do { \
      double b = pop(); \
      double a = pop(); \
      push(a op b); \
    } while (false)

        for (;;) {
#ifdef DEBUG_TRACE_EXECUTION

            printf("          ");
            for (Const::Value *slot = vm.stack; slot < vm.stackTop; slot++) {
                printf("[ ");
                Const::printValue(*slot);
                printf(" ]");
            }
            printf("\n");

            Debug::disassembleInstruction(vm.chunk, (int) (vm.ip - vm.chunk->code));
#endif
            switch (uint8_t instruction = READ_BYTE()) {
                case Chunk::OP_CONSTANT: {
                    Const::Value constant = READ_CONSTANT();
                    push(constant);
                    break;
                }

                case Chunk::OP_ADD: BINARY_OP(+);
                    break;
                case Chunk::OP_SUBTRACT: BINARY_OP(-);
                    break;
                case Chunk::OP_MULTIPLY: BINARY_OP(*);
                    break;
                case Chunk::OP_DIVIDE: BINARY_OP(/);
                    break;
                case Chunk::OP_NEGATE:
                    push(-pop());
                    break;
                case Chunk::OP_RETURN: {
                    Const::printValue(pop());
                    printf("\n");
                    return INTERPRET_OK;
                }
            }
        }
#undef READ_CONSTANT
#undef BINARY_OP
#undef READ_BYTE
    }

    InterpretResult interpret(const char* source) {
        Compiler::compile(source);
        return INTERPRET_OK;
    }

    void freeVM() {
        Chunk::freeChunk(vm.chunk);
    }
}
