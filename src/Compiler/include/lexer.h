//
// Created by 15813 on 2026/7/17.
//

#ifndef TOYPROJECTS_COMPILER_LEXER_H
#define TOYPROJECTS_COMPILER_LEXER_H

#include "../include/token.h"
#include <iostream>

namespace Compiler::Lexer {
    typedef struct LEXER_STRUCT{
        char* src;
        std::size_t src_size;
        char c;
        unsigned int i;
    }lexer_T;

    lexer_T* init_lexer(char* src);
    void lexer_advance(lexer_T* lexer);
    char lexer_peek(lexer_T* lexer, int offset);
    Token::token_T* lexer_advance_with(Lexer::lexer_T* lexer, Token::token_T* token); // 跑到下一个lexer，原样返回token
    Token::token_T* lexer_advance_current(Lexer::lexer_T* lexer, Token::TOKEN_STRUCT::TokenType type); // 跑到下一个lexer，用当前指向的lexer包装token
    void lexer_skip_whitespace(lexer_T* lexer);
    Token::token_T* lexer_parse_id(lexer_T* lexer);
    Token::token_T* lexer_parse_number(lexer_T* lexer);
    Token::token_T* lexer_next_token(lexer_T* lexer);


}
#endif //TOYPROJECTS_COMPILER_LEXER_H
