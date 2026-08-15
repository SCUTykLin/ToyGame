//
// Created by YangKai.Lin on 2026/8/5.
//

#include "App.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

namespace CLOX {
    void App::run() {
        Vm::initVM();
        Chunk::Chunk chunk;
        initChunk(&chunk);
        int constant = addConstant(&chunk, 1.2);
        writeChunk(&chunk, Chunk::OP_CONSTANT, 123);
        writeChunk(&chunk, constant, 123);

        constant = addConstant(&chunk, 3.4);
        writeChunk(&chunk, Chunk::OP_CONSTANT, 123);
        writeChunk(&chunk, constant, 123);

        writeChunk(&chunk, Chunk::OP_ADD, 123);

        constant = addConstant(&chunk, 5.6);
        writeChunk(&chunk, Chunk::OP_CONSTANT, 123);
        writeChunk(&chunk, constant, 123);

        writeChunk(&chunk, Chunk::OP_DIVIDE, 123);

        writeChunk(&chunk, Chunk::OP_NEGATE, 123);
        writeChunk(&chunk, Chunk::OP_RETURN, 123);
        Debug::disassembleChunk(&chunk, "test chunk");

        Vm::interpret(&chunk);
        Vm::freeVM();
        freeChunk(&chunk);
    }
}