//
// Created by 15813 on 2026/8/15.
//

#ifndef TOYPROJECTS_CLOX_COMPILER_H
#define TOYPROJECTS_CLOX_COMPILER_H

#include "chunk.h"

namespace CLOX::Compiler{
    bool compile(const char* source, Chunk::Chunk* chunk);
}

#endif //TOYPROJECTS_CLOX_COMPILER_H
