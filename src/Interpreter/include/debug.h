//
// Created by 15813 on 2026/8/15.
//

#ifndef TOYPROJECTS_CLOX_DEBUG_H
#define TOYPROJECTS_CLOX_DEBUG_H

#include "chunk.h"

namespace CLOX::Debug{
    void disassembleChunk(Chunk::Chunk* chunk, const char* name);
    int disassembleInstruction(Chunk::Chunk* chunk, int offset);
}
#endif //TOYPROJECTS_CLOX_DEBUG_H
