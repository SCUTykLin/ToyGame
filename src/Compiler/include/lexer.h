//
// Created by 15813 on 2026/7/17.
//

#ifndef TOYPROJECTS_COMPILER_LEXER_H
#define TOYPROJECTS_COMPILER_LEXER_H
namespace Compiler::lexer {
    typedef struct LEXER_STRUCT{
        char* src;
        char c;
        unsigned int i;
    }lexer_T;

    lexer_T* init_lexer(char* src);
}
#endif //TOYPROJECTS_COMPILER_LEXER_H
