//
// Created by YangKai.Lin on 2026/11/1.
//

#include "chunk.h"

namespace CLOX_V2 {
    class App {
    public:
        void run() {
            Chunk::Chunk chunk;
            Chunk::initChunk(&chunk);
            Chunk::writeChunk(&chunk, Chunk::OP_RETURN);
            Chunk::disassembleChunk(&chunk, "test chunk");
            Chunk::freeChunk(&chunk);
        }
    };
}
