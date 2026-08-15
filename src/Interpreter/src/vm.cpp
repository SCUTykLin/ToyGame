//
// Created by 15813 on 2026/8/15.
//

#include "common.h"
#include "vm.h"
#include "debug.h"

namespace CLOX::Vm{

    VM vm;

    static void resetStack() {
        vm.stackTop = vm.stack;
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
            for (Value::Value* slot = vm.stack; slot < vm.stackTop; slot++) {
                printf("[ ");
                Value::printValue(*slot);
                printf(" ]");
            }
            printf("\n");
            Debug::disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
            uint8_t instruction;
            switch (instruction = READ_BYTE()) {
                case Chunk::OP_CONSTANT: {
                    Value::Value constant = READ_CONSTANT();
                    push(constant);
                    break;
                }
                case Chunk::OP_ADD:      BINARY_OP(+); break;
                case Chunk::OP_SUBTRACT: BINARY_OP(-); break;
                case Chunk::OP_MULTIPLY: BINARY_OP(*); break;
                case Chunk::OP_DIVIDE:   BINARY_OP(/); break;
                case Chunk::OP_NEGATE:
                    push(-pop());
                    break;
                case Chunk::OP_RETURN: {
                    Value::printValue(pop());
                    printf("\n");
                    return INTERPRET_OK;
                }
            }
        }
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
    }

    void initVM() {
        resetStack();
    }

    void freeVM() {
    }

    InterpretResult interpret(Chunk::Chunk *chunk) {
        vm.chunk = chunk;
        vm.ip = vm.chunk->code;
        return run();
    }

    void push(Value::Value value) {
        *vm.stackTop = value;
        vm.stackTop++;
    }

    Value::Value pop() {
        vm.stackTop--;
        return *vm.stackTop;
    }

}
