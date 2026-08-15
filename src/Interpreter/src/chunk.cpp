//
// Created by 15813 on 2026/8/15.
//

#include "chunk.h"
#include "clox_memory.h"
#include <vector>

namespace CLOX::Chunk{

    int getLine(Chunk* chunk, int index) {
        auto lines = decode(chunk);
        return lines[index];
    }

    void addLine(Chunk* chunk, int line) {
        if (chunk->lineRun_length == 0){
            chunk->lines[chunk->lineRun_length++] = {.line = line, .length = 1};
        } else if(chunk->lines[chunk->lineRun_length-1].line == line){
            chunk->lines[chunk->lineRun_length-1].length ++;
        } else{
            chunk->lines[chunk->lineRun_length++] = {.line = line, .length = 1};
        }
    }

    void initChunk(Chunk* chunk){
        chunk->count = 0;
        chunk->capacity = 0;
        chunk->lineRun_length = 0;
        chunk->code = nullptr;
        chunk->lines = nullptr;
        initValueArray(&chunk->constants);
    }

    void writeChunk(Chunk* chunk, uint8_t byte, int line) {
        if (chunk->capacity < chunk->count + 1){
            int oldCapacity = chunk->capacity;
            chunk->capacity = GROW_CAPACITY(oldCapacity);
            chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
            chunk->lines = GROW_ARRAY(lineRun, chunk->lines, oldCapacity, chunk->capacity);
        }
        chunk->code[chunk->count] = byte;
        addLine(chunk, line);
        ++ chunk->count;
    }

    int addConstant(Chunk* chunk, Value::Value value){
        writeValueArray(&chunk->constants, value);
        return chunk->constants.count - 1;
    }

    void freeChunk(Chunk* chunk){
        FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
        FREE_ARRAY(lineRun , chunk->lines, chunk->capacity);
        freeValueArray(&chunk->constants);
        initChunk(chunk);
    }

    std::vector<int> decode(Chunk* chunk){
        std::vector<int> decode_line;
        for (int i = 0; i < chunk->lineRun_length; i++){
            for (int j = 0; j < chunk->lines[i].length; ++j)
                decode_line.push_back(chunk->lines[i].line);
        }
        return decode_line;
    }
}