//
// Created by YangKai.Lin on 2026/11/3.
//
#include "compiler.h"

#include <cstdio>

#include "common.h"
#include "scanner.h"

namespace CLOX_V2::Compiler {
    void compile(const char* source) {
        Scan::initScanner(source);

        int line = -1;
        for (;;) {
            Scan::Token token = Scan::scanToken();
            if (token.line != line) {
                printf("%4d ", token.line);
                line = token.line;
            }else {
                printf("   | ");
            }
            printf("%2d '%.*s'\n", token.type, token.length, token.start);
            if (token.type == Scan::TOKEN_EOF) break;
        }
    }
}