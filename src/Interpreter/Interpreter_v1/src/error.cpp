//
// Created by 15813 on 2026-11-01.
//
#include "error.h"
#include "Token.h"

namespace CLOX_V1::Err {
    void error(const Token::Token& token, const std::string &message) {
        if (token.type == Token::EOF_Type) {
            report(token.line, " at end", message);
        } else {
            report(token.line, " at '" + token.lexeme + "'", message);
        }
    }

    void runtimeError(const Token::Token& token, const std::string &message) {
        std::cerr << message << "\n[line " << token.line << "]" << std::endl;
    }
}