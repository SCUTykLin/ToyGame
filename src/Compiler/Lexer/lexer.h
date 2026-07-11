//
// Created by YangKai.Lin on 2026/7/11.
//

#ifndef TOYPROJECTS_LEXER_H
#define TOYPROJECTS_LEXER_H
#include <cstdio>
#include "../compiler/compiler.h"

namespace compiler {
    class Lexer {
    public:
        Lexer(FILE* file);
    private:
        TokenLiteral* generate_number(char current, FILE *file);
        TokenKeyword* generate_keyword(char current, FILE* file);
    };
}

#endif //TOYPROJECTS_LEXER_H