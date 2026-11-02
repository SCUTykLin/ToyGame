//
// Created by YangKai.Lin on 2026/11/2.
//

#ifndef TOYPROJECTS_Interpreter_v2_DEBUG_H
#define TOYPROJECTS_Interpreter_v2_DEBUG_H
#include "chunk.h"

namespace CLOX_V2::Debug {
    // 反编译这个Chunk里面的所有指令
    void disassembleChunk(Chunk::Chunk* chunk, const char* name);
    // 遍历一条指令
    int disassembleInstruction(Chunk::Chunk* chunk, int offset);
}

#endif //TOYPROJECTS_Interpreter_v2_DEBUG_H
