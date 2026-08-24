//
// Created by 15813 on 2026/7/17.
//

#include "../include/lexer.h"
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include "../include/macros.h"
#include "../include/token.h"

namespace Compiler{
    Lexer::lexer_T* Lexer::init_lexer(char* src){
        Lexer::lexer_T* lexer = static_cast<Lexer::lexer_T*>(calloc(1, sizeof(struct Lexer::LEXER_STRUCT)));
        lexer->src = src;
        lexer->src_size = strlen(src);
        lexer->i = 0;
        lexer->c = lexer->src[lexer->i];
        return lexer;
    }

    void Lexer::lexer_advance(Lexer::lexer_T* lexer) {
        if (lexer->i < lexer->src_size && lexer->c != '\0') {
            lexer->c = lexer->src[++lexer->i];
        }
    }

    char Lexer::lexer_peek(Lexer::lexer_T* lexer, int offset) {
        return lexer->src[__MIN__(lexer->i + offset, lexer->src_size)];
    }

    Token::token_T* Lexer::lexer_advance_with(Lexer::lexer_T* lexer, Token::token_T* token) {
        Lexer::lexer_advance(lexer);
        return token;
    }

    Token::token_T* Lexer::lexer_advance_current(Lexer::lexer_T* lexer, Token::TOKEN_STRUCT::TokenType type) {
        char* value = static_cast<char*>(calloc(2, sizeof(char)));
        value[0] = lexer->c;
        value[1] = '\0';
        Token::token_T* token = Token::init_token(value, type);
        lexer_advance(lexer);
        return token;
    }

    void Lexer::lexer_skip_whitespace(Lexer::lexer_T* lexer) {
        while (lexer->c == 13 || lexer->c == 10 || lexer->c == ' ' || lexer->c == '\t' || lexer->c == '\n') {
            Lexer::lexer_advance(lexer);
        }
    }

    Token::token_T* Lexer::lexer_parse_id(Lexer::lexer_T* lexer) {
        char* value = static_cast<char*>(calloc(1, sizeof(char)));
        while (isalpha(lexer->c)) {
            value = static_cast<char*>(realloc(value, (strlen(value) + 2) * sizeof(char)));
            strcat(value, std::string{lexer->c, '\0'}.c_str());
            Lexer::lexer_advance(lexer);
        }
        return Token::init_token(value, Token::TOKEN_STRUCT::TOKEN_ID);
    }

    Token::token_T* Lexer::lexer_parse_number(lexer_T* lexer) {
        char* value = static_cast<char*>(calloc(1, sizeof(char)));
        while (isdigit(lexer->c)) {
            value = static_cast<char*>(realloc(value, (strlen(value) + 2) * sizeof(char)));
            strcat(value, std::string{lexer->c, '\0'}.c_str());
            Lexer::lexer_advance(lexer);
        }
        return Token::init_token(value, Token::TOKEN_STRUCT::TOKEN_INT);
    }

    Token::token_T* Lexer::lexer_next_token(Lexer::lexer_T* lexer) {
        while (lexer->c != '\0') {
            Lexer::lexer_skip_whitespace(lexer);
            if (isalpha(lexer->c)) {
                return Lexer::lexer_parse_id(lexer);
            }
            if (isdigit(lexer->c)) {
                return Lexer::lexer_parse_number(lexer);
            }
            switch (lexer->c) {
                case '=': {
                    if (Lexer::lexer_peek(lexer, 1) == '>') // =>
                        return Lexer::lexer_advance_with(lexer, Lexer::lexer_advance_with(lexer, Token::init_token("=>", Token::TOKEN_STRUCT::TOKEN_ARROW_RIGHT)));
                    return Lexer::lexer_advance_with(lexer, Token::init_token("=", Token::TOKEN_STRUCT::TOKEN_EQUALS)); // ==
                }
                case '(': {
                    return Lexer::lexer_advance_current(lexer, Token::TOKEN_STRUCT::TOKEN_LPAREN);
                }
                case ')': {
                    return Lexer::lexer_advance_current(lexer, Token::TOKEN_STRUCT::TOKEN_RPAREN);
                }
                case '{': {
                    return Lexer::lexer_advance_current(lexer, Token::TOKEN_STRUCT::TOKEN_LBRACE);
                }
                case '}': {
                    return Lexer::lexer_advance_current(lexer, Token::TOKEN_STRUCT::TOKEN_RBRACE);
                }
                case ':': {
                    return Lexer::lexer_advance_current(lexer, Token::TOKEN_STRUCT::TOKEN_COLON);
                }
                case ',': {
                    return Lexer::lexer_advance_current(lexer, Token::TOKEN_STRUCT::TOKEN_COMMA);
                }
                case '<': {
                    return Lexer::lexer_advance_current(lexer, Token::TOKEN_STRUCT::TOKEN_LT);
                }
                case '>': {
                    return Lexer::lexer_advance_current(lexer, Token::TOKEN_STRUCT::TOKEN_GT);
                }
                case ';': {
                    return Lexer::lexer_advance_current(lexer, Token::TOKEN_STRUCT::TOKEN_SEMI);
                }
                case '\0': break;
                default: {printf("[Lexer]: Unexpected character: %d\n", lexer->c); exit(1); break;}
            }
        }
        return Token::init_token(0, Token::TOKEN_STRUCT::TOKEN_EOF);
    }
}