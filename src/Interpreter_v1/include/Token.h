//
// Created by 15813 on 2026/8/21.
//

#ifndef TOYPROJECTS_Interpreter_v1_TOKEN_H
#define TOYPROJECTS_Interpreter_v1_TOKEN_H

#include "TokenType.h"
#include <string>
#include <any>

namespace CLOX_V1::Token {
    class Token{
    public:
        Token(TokenType type, std::string lexeme, std::any literal, int line);
        std::string toString();
        std::string type2str(TokenType_t type);

    public:
        TokenType type;
        std::string lexeme;
        std::any literal;
        int line;
    };
}


#endif //TOYPROJECTS_Interpreter_v1_TOKEN_H
