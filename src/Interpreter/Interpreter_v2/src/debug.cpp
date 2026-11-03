//
// Created by YangKai.Lin on 2026/11/2.
//

#include "debug.h"
#include "value.h"
#include <cstdio>

namespace CLOX_V2::Debug {
    static int constantInstruction(const char *name, Chunk::Chunk *chunk, int offset) {
        uint8_t constant = chunk->code[offset + 1];
        printf("%-16s %4d '", name, constant);
        Const::printValue(chunk->constants.values[constant]);
        printf("'\n");
        return offset + 2;
    }

    static int simpleInstruction(const char *name, int offset) {
        printf("%s\n", name);
        return offset + 1;
    }

    void disassembleChunk(Chunk::Chunk *chunk, const char *name) {
        printf("== %s ==\n", name);
        for (int offset = 0; offset < chunk->count;) {
            offset = disassembleInstruction(chunk, offset);
        }
    }

    int disassembleInstruction(Chunk::Chunk *chunk, int offset) {
        printf("%04d ", offset);

        if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
            printf("   | ");
        }else {
            printf("%4d ", chunk->lines[offset]);
        }

        uint8_t instruction = chunk->code[offset];
        switch (instruction) {
            case Chunk::OP_CONSTANT:
                return constantInstruction("OP_CONSTANT", chunk, offset);
            case Chunk::OP_ADD:
                return simpleInstruction("OP_ADD", offset);
            case Chunk::OP_SUBTRACT:
                return simpleInstruction("OP_SUBTRACT", offset);
            case Chunk::OP_MULTIPLY:
                return simpleInstruction("OP_MULTIPLY", offset);
            case Chunk::OP_DIVIDE:
                return simpleInstruction("OP_DIVIDE", offset);
            case Chunk::OP_NEGATE:
                return simpleInstruction("OP_NEGATE", offset);
            case Chunk::OP_RETURN:
                return simpleInstruction("OP_RETURN", offset);
            default:
                printf("Unknown (opcode)instruction %d\n", instruction);
                return offset + 1;
        }
    }
}
