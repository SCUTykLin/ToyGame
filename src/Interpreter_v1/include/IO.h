//
// Created by 15813 on 2026/8/21.
//

#ifndef TOYPROJECTS_Interpreter_v1_IO_H
#define TOYPROJECTS_Interpreter_v1_IO_H

#include <iostream>
#include <string>

namespace CLOX_V1::IO{
    class IOSystem{
    public:
        static std::string readAllBytes(const char* filename);
    };
}

#endif //TOYPROJECTS_Interpreter_v1_IO_H
