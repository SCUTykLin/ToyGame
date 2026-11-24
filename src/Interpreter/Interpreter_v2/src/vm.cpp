//
// Created by YangKai.Lin on 2026/11/3.
//
#include "common.h"
#include "vm.h"
#include "debug.h"
#include <cstdio>
#include "compiler.h"
#include <cstdarg>   // 加到文件顶部

namespace CLOX_V2::VM {
    VM vm;

    static void resetStack() {
        vm.stackTop = vm.stack;
    }

    static void runtimeError(const char *format, ...) {
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        fputs("\n", stderr);

        size_t instruction = vm.ip - vm.chunk->code - 1;
        int line = vm.chunk->lines[instruction];
        fprintf(stderr, "[line %d] in script\n", line);

        resetStack();
    }

    void initVM() {
        resetStack();
    }

    void push(Const::Value value) {
        *vm.stackTop = value;
        vm.stackTop++;
    }

    static Const::Value peek(int distance) {
        return vm.stackTop[-1 - distance];
    }

    static bool isFalsey(Const::Value value) {
        return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
    }

    Const::Value pop() {
        vm.stackTop--;
        return *vm.stackTop;
    }

    static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(valueType, op) \
    do { \
      if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
        runtimeError("Operands must be numbers."); \
        return INTERPRET_RUNTIME_ERROR; \
      } \
      double b = AS_NUMBER(pop()); \
      double a = AS_NUMBER(pop()); \
      push(valueType(a op b)); \
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

                case Chunk::OP_NIL: push(NIL_VAL);
                    break;
                case Chunk::OP_TRUE: push(BOOL_VAL(true));
                    break;
                case Chunk::OP_FALSE: push(BOOL_VAL(false));
                    break;
                case Chunk::OP_EQUAL: {
                    Const::Value b = pop();
                    Const::Value a = pop();
                    push(BOOL_VAL(valuesEqual(a, b)));
                    break;
                }
                case Chunk::OP_GREATER: BINARY_OP(BOOL_VAL, >);
                    break;
                case Chunk::OP_LESS: BINARY_OP(BOOL_VAL, <);
                    break;
                case Chunk::OP_ADD: BINARY_OP(NUMBER_VAL, +);
                    break;
                case Chunk::OP_SUBTRACT: BINARY_OP(NUMBER_VAL, -);
                    break;
                case Chunk::OP_MULTIPLY: BINARY_OP(NUMBER_VAL, *);
                    break;
                case Chunk::OP_DIVIDE: BINARY_OP(NUMBER_VAL, /);
                    break;
                case Chunk::OP_NOT:
                    push(BOOL_VAL(isFalsey(pop())));
                    break;
                case Chunk::OP_NEGATE: {
                    if (!IS_NUMBER(peek(0))) {
                        runtimeError("Operand must be a number.");
                        return INTERPRET_RUNTIME_ERROR;
                    }
                    push(NUMBER_VAL(-AS_NUMBER(pop())));
                    break;
                }
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

    InterpretResult interpret(const char *source) {
        Chunk::Chunk chunk;
        Chunk::initChunk(&chunk);
        if (!Compiler::compile(source, &chunk)) {
            Chunk::freeChunk(&chunk);
            return INTERPRET_COMPILE_ERROR;
        }

        vm.chunk = &chunk;
        vm.ip = vm.chunk->code; // 指向指令
        InterpretResult result = run();
        Chunk::freeChunk(&chunk);
        return result;
    }

    void freeVM() {
    }
}
