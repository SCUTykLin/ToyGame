//
// Created by YangKai.Lin on 2026/8/5.
//

#ifndef TOYPROJECTS_CLOX_CHUNK_H
#define TOYPROJECTS_CLOX_CHUNK_H

#include "common.h"

typedef enum {
    OP_RETURN,
}OpCode;

typedef struct {
    uint8_t* code;
} Chunk;

#endif //TOYPROJECTS_CLOX_CHUNK_H