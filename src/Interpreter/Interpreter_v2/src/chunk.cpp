//
// Created by 15813 on 2026-11-02.
//

#include "chunk.h"
namespace CLOX_V2::Chunk {
    void initChunk(Chunk *chunk) {
        chunk->count = 0;
        chunk->capacity = 0;
        chunk->code = nullptr;
        chunk->lines = nullptr;
        Const::initValueArray(&chunk->constants);
    }

    void writeChunk(Chunk *chunk, uint8_t byte, int line) {
        if (chunk->capacity < chunk->count + 1) {
            int oldCapacity = chunk->capacity;
            chunk->capacity = GROW_CAPACITY(oldCapacity);
            chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
            chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
        }

        chunk->code[chunk->count] = byte;
        chunk->lines[chunk->count] = line;
        chunk->count++;
    }

    void freeChunk(Chunk* chunk) {
        FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
        FREE_ARRAY(int, chunk->lines, chunk->capacity);
        Const::freeValueArray(&chunk->constants);
        initChunk(chunk);
    }

    int addConstant(Chunk *chunk, Const::Value value) {
        Const::writeValueArray(&chunk->constants, value);
        return chunk->constants.count - 1;
    }
}
