//
// Created by 15813 on 2026/7/17.
//

#ifndef TOYPROJECTS_COMPILER_TOKEN_H
#define TOYPROJECTS_COMPILER_TOKEN_H
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace Compiler::Token {
    typedef struct TOKEN_STRUCT {
        const char *value;
        enum TokenType{
            TOKEN_ID, // main
            TOKEN_EQUALS, // =
            TOKEN_LPAREN, // (
            TOKEN_RPAREN, // )
            TOKEN_LBRACE, // {
            TOKEN_RBRACE, // }
            TOKEN_COLON, // :
            TOKEN_COMMA, // ,
            TOKEN_LT, // <
            TOKEN_GT, // >
            TOKEN_ARROW_RIGHT, // =>
            TOKEN_INT, // number
            TOKEN_SEMI, // ;
            TOKEN_EOF
        };
        TokenType type;
    } token_T;

    token_T* init_token(const char* value, TOKEN_STRUCT::TokenType type);
    inline const char* token_type_to_str(const TOKEN_STRUCT::TokenType type) {
        switch (type) {
            case TOKEN_STRUCT::TOKEN_ID: return "TOKEN_ID";
            case TOKEN_STRUCT::TOKEN_EQUALS: return "TOKEN_EQUALS";
            case TOKEN_STRUCT::TOKEN_LPAREN: return "TOKEN_LPAREN";
            case TOKEN_STRUCT::TOKEN_RPAREN: return "TOKEN_RPAREN";
            case TOKEN_STRUCT::TOKEN_LBRACE: return "TOKEN_LBRACE";
            case TOKEN_STRUCT::TOKEN_RBRACE: return "TOKEN_RBRACE";
            case TOKEN_STRUCT::TOKEN_COLON: return "TOKEN_COLON";
            case TOKEN_STRUCT::TOKEN_COMMA: return "TOKEN_COMMA";
            case TOKEN_STRUCT::TOKEN_LT: return "TOKEN_LT";
            case TOKEN_STRUCT::TOKEN_GT: return "TOKEN_GT";
            case TOKEN_STRUCT::TOKEN_ARROW_RIGHT: return "TOKEN_ARROW_RIGHT";
            case TOKEN_STRUCT::TOKEN_INT: return "TOKEN_INT";
            case TOKEN_STRUCT::TOKEN_SEMI: return "TOKEN_SEMI";
            case TOKEN_STRUCT::TOKEN_EOF: return "TOKEN_EOF";
            default: return "UNKNOWN";
        }
    }

    inline const char* token_to_str(token_T* token) {
        const char* type_str = token_type_to_str(token->type);
        const char* template_type = "<type = %s, int_type = %d, value = %s>\n";
        char* str = static_cast<char*>(calloc(strlen(type_str) + strlen(template_type) + 8, sizeof(char)));
        sprintf(str, template_type, type_str, token->type, token->value);
        return str;
    }
}

#endif //TOYPROJECTS_COMPILER_TOKEN_H
