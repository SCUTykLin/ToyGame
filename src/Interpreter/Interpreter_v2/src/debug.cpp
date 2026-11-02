//
// Created by YangKai.Lin on 2026/11/2.
//

#include "debug.h"
#include <cstdio>

namespace CLOX_V2::Debug {
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
        uint8_t instruction = chunk->code[offset];
        switch (instruction) {
            case Chunk::OP_RETURN:
                return simpleInstruction("OP_RETURN", offset);
            default:
                printf("Unknown (opcode)instruction %d\n", instruction);
                return offset + 1; // 有些指令可能是多几行code的
        }
    }
}
