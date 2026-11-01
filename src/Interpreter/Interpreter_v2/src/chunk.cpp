//
// Created by 15813 on 2026-11-02.
//

#include "chunk.h"
namespace CLOX_V2::Chunk {
    void initChunk(Chunk *chunk) {
        chunk->count = 0;
        chunk->capacity = 0;
        chunk->code = nullptr;
    }

    void writeChunk(Chunk *chunk, uint8_t byte) {
        if (chunk->capacity < chunk->count + 1) {
            int oldCapacity = chunk->capacity;
            chunk->capacity = GROW_CAPACITY(oldCapacity);
            chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
        }

        chunk->code[chunk->count] = byte;
        chunk->count++;
    }

    void freeChunk(Chunk* chunk) {
        FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
        initChunk(chunk);
    }
}
