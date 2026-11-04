//
// Created by YangKai.Lin on 2026/11/3.
//

#ifndef TOYPROJECTS_Interpreter_v2_COMPILER_H
#define TOYPROJECTS_Interpreter_v2_COMPILER_H

#include "chunk.h"

namespace CLOX_V2::Compiler {
    bool compile(const char* source, Chunk::Chunk* chunk);
}

#endif //TOYPROJECTS_Interpreter_v2_COMPILER_H
