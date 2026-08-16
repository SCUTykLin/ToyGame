//
// Created by 15813 on 2026/8/15.
//
#include <cstdio>
#include "common.h"
#include "compiler.h"
#include "scanner.h"

namespace CLOX::Compiler{
    void compile(const char* source) {
        Scan::initScanner(source);

        int line = -1;
        for (;;) {
            Scan::Token token = Scan::scanToken();
            if (token.line != line) {
                printf("%4d ", token.line);
                line = token.line;
            } else {
                printf("   | ");
            }
            printf("%2d '%.*s'\n", token.type, token.length, token.start);

            if (token.type == Scan::TOKEN_EOF) break;
        }
    }
}
