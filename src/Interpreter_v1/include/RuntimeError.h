//
// Created by YangKai.Lin on 2026/11/1.
//

#ifndef TOYPROJECTS_Interpreter_v1_RUNTIMEERROR_H
#define TOYPROJECTS_Interpreter_v1_RUNTIMEERROR_H

#include <stdexcept>
#include <string>

#include "Token.h"

namespace CLOX_V1 {
    class RuntimeError : public std::runtime_error {
    public:
        RuntimeError(const Token::Token& token, const std::string& message)
            : std::runtime_error(message), token(token) {}

        const Token::Token token;
    };
}

#endif //TOYPROJECTS_Interpreter_v1_RUNTIMEERROR_H
