//
// Created by YangKai.Lin on 2026/7/17.
//

#include "../include/io.h"
#include <fstream>
#include <iostream>
#include <string>

namespace Compiler {
    namespace IO {
        std::string tac_read_file(const char* filename) {
            std::ifstream fp(filename);
            std::string line;
            std::string buffer{""};

            while (std::getline(fp, line)) {
                buffer += line;
            }

            fp.close();
            return buffer;
        }
    }
}
