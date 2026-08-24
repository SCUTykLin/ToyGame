//
// Created by 15813 on 2026/8/21.
//

#ifndef TOYPROJECTS_Interpreter_v1_COMPILER_H
#define TOYPROJECTS_Interpreter_v1_COMPILER_H

#include <string>

namespace CLOX_V1::Compiler{
    class CompilerSystem{
    public:
        void run(const std::string& source_code);
        inline bool hasError() {return hadError;}
    private:
        bool hadError = false;
    };
}

#endif //TOYPROJECTS_Interpreter_v1_COMPILER_H
