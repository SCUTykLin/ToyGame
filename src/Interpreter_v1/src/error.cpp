//
// Created by 15813 on 2026-11-01.
//
#include "error.h"
#include "token.h"

namespace CLOX_V1::Err {
    void error(const Token::Token& token, const std::string &message) {
        if (token.type == Token::EOF_Type) {
            report(token.line, " at end", message);
        } else {
            report(token.line, " at '" + token.lexeme + "'", message);
        }
    }
}