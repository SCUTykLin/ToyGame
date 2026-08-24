//
// Created by 15813 on 2026/8/21.
//

#ifndef TOYPROJECTS_Interpreter_v1_TOKENTYPE_H
#define TOYPROJECTS_Interpreter_v1_TOKENTYPE_H

#include <string>

namespace CLOX_V1::Token {
    typedef enum TokenType {
        // Single-character tokens.
        LEFT_PAREN = 0, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
        COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

        // One or two character tokens.
        BANG, BANG_EQUAL,
        EQUAL, EQUAL_EQUAL,
        GREATER, GREATER_EQUAL,
        LESS, LESS_EQUAL,

        // Literals.
        IDENTIFIER, STRING, NUMBER,

        // Keywords.
        AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
        PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

        // Comments
        COMMENTS,

        EOF_Type
    } TokenType_t;
}

#endif //TOYPROJECTS_Interpreter_v1_TOKENTYPE_H
