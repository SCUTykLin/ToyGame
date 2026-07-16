//
// Created by 15813 on 2026/7/17.
//

#include "../include/lexer.h"
#include <stdlib.h>

namespace Compiler::lexer{
    lexer_T* init_lexer(char* src){
        lexer_T* lexer = static_cast<lexer_T*>(calloc(1, sizeof(struct LEXER_STRUCT)));
        lexer->src = src;
        lexer->i = 0;
        lexer->c = lexer->src[lexer->i];
        return lexer;
    }
}