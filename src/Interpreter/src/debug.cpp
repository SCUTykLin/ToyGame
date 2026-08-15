//
// Created by 15813 on 2026/8/15.
//
#include "debug.h"
#include <cstdio>
#include "value.h"

static int simpleInstruction(const char* name, int offset) {
    printf("%s\n", name);
    return offset + 1;
}

static int constantInstruction(const char* name, CLOX::Chunk::Chunk* chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1]; // 当前常数所在的位置
    printf("%-16s %4d '", name, constant);
    CLOX::Value::printValue(chunk->constants.values[constant]);
    printf("'\n");
    return offset + 2;
}

void CLOX::Debug::disassembleChunk(CLOX::Chunk::Chunk *chunk, const char *name) {
    printf("== %s ==\n", name);
    for (int offset = 0; offset < chunk->count;) {
        offset = disassembleInstruction(chunk, offset);
    }
}

int CLOX::Debug::disassembleInstruction(CLOX::Chunk::Chunk *chunk, int offset) {
    printf("%04d ", offset);

    // 显式行号
    if (offset > 0 && Chunk::getLine(chunk, offset) == Chunk::getLine(chunk, offset-1)) {
        printf("   | ");
    } else {
        printf("%4d ", Chunk::getLine(chunk, offset));
    }

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case Chunk::OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
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
        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}

