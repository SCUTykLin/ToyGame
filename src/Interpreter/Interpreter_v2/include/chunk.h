//
// Created by 15813 on 2026-11-02.
//

#ifndef TOYPROJECTS_Interpreter_v2_CHUNK_H
#define TOYPROJECTS_Interpreter_v2_CHUNK_H
#include <cstdint>
#include "chunk_memory.h"
#include "value.h"

namespace CLOX_V2::Chunk {
    // 字节码
    typedef enum {
        OP_CONSTANT,
        OP_ADD,
        OP_SUBTRACT,
        OP_MULTIPLY,
        OP_DIVIDE,
        OP_NEGATE,
        OP_RETURN,
    } OpCode;

    // 指令动态数组，存储指令
    typedef struct {
        int count;
        int capacity;
        uint8_t *code;
        int *lines;
        Const::ValueArray constants;
    } Chunk;

    void initChunk(Chunk *chunk);

    void freeChunk(Chunk *chunk);

    void writeChunk(Chunk *chunk, uint8_t byte, int line);

    int addConstant(Chunk *chunk, Const::Value value);
}

#endif //TOYPROJECTS_Interpreter_v2_CHUNK_H
