//
// Created by YangKai.Lin on 2026/8/5.
//

#ifndef TOYPROJECTS_CLOX_CHUNK_H
#define TOYPROJECTS_CLOX_CHUNK_H

#include "common.h"
#include "value.h"
#include <vector>

namespace CLOX::Chunk {

    typedef struct {
        int line;
        int length;
    }lineRun;

    typedef enum {
        OP_CONSTANT,
        OP_ADD,
        OP_SUBTRACT,
        OP_MULTIPLY,
        OP_DIVIDE,
        OP_NEGATE,
        OP_RETURN,
    } OpCode;

    typedef struct {
        int count;
        int capacity;
        uint8_t *code;
        int* long_code;

        lineRun* lines;
        int lineRun_length;

        Value::ValueArray constants;
    } Chunk;

    void initChunk(Chunk* chunk);

    void writeChunk(Chunk* chunk, uint8_t byte, int line);

    int addConstant(Chunk* chunk, Value::Value value);

    void freeChunk(Chunk* chunk);

    int getLine(Chunk* chunk, int index);

    void addLine(Chunk* chunk, int line);

    std::vector<int> decode(Chunk* chunk);
}

#endif //TOYPROJECTS_CLOX_CHUNK_H