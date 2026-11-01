//
// Created by 15813 on 2026-11-02.
//

#ifndef TOYPROJECTS_Interpreter_v2_CHUNK_H
#define TOYPROJECTS_Interpreter_v2_CHUNK_H
#include <cstdint>
#include "chunk_memory.h"

namespace CLOX_V2::Chunk {
    // 字节码
    typedef enum {
        OP_RETURN,
    }OpCode;

    // 指令动态数组，存储指令
    typedef struct {
        int count;
        int capacity;
        uint8_t* code;
    }Chunk;

    void initChunk(Chunk* chunk);
    void freeChunk(Chunk* chunk);

    void writeChunk(Chunk* chunk, uint8_t byte);
}

#endif //TOYPROJECTS_Interpreter_v2_CHUNK_H
