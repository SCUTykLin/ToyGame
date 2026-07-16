//
// Created by 15813 on 2026/7/17.
//

#ifndef TOYPROJECTS_COMPILER_TOKEN_H
#define TOYPROJECTS_COMPILER_TOKEN_H
namespace Compiler::token {
    typedef struct TOKEN_STRUCT {
        char *value;
        enum type{
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
            TOKEN_RIGHT_ARROW, // =>
            TOKEN_INT, // number
            TOKEN_SEMI, // ;
        } type;
    } token_T;

    token_T* init_token(char* value, enum type type);
}

#endif //TOYPROJECTS_COMPILER_TOKEN_H
