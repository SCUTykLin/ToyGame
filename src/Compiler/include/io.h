//
// Created by YangKai.Lin on 2026/7/17.
//

#ifndef TOYPROJECTS_COMPILER_IO_H
#define TOYPROJECTS_COMPILER_IO_H
#include <string>

namespace Compiler {
    namespace IO {
        std::string tac_read_file(const char* filename);
        void tac_write_file(const char* filename, char* outbuffer);
    }
}

#endif //TOYPROJECTS_IO_H