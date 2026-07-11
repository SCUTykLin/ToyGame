//
// Created by YangKai.Lin on 2026/7/11.
//

#ifndef TOYPROJECTS_APP_H
#define TOYPROJECTS_APP_H

#include <iostream>
#include <stdio.h>

#include "Lexer/lexer.h"

namespace compiler {
    class CompilerApp {
    public:
        void run_compile(std::string fileName);
        FILE* openFile(std::string fileName);
    private:
        Lexer* lexerPtr = nullptr;
    };
}

#endif //TOYPROJECTS_APP_H