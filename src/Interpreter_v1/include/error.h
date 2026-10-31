//
// Created by 15813 on 2026/8/21.
//

#ifndef TOYPROJECTS_Interpreter_v1_ERROR_H
#define TOYPROJECTS_Interpreter_v1_ERROR_H

#include <iostream>
#include <format>

namespace CLOX_V1::Token {
    class Token;
};

namespace CLOX_V1::Err {
    void report(int line, const std::string &where, const std::string &message) {
        std::cout << std::format("[line {}] Error {}: {}", line, where, message) << std::endl;
    }

    void error(int line, const std::string &message) {
        report(line, "", message);
    }

    void error(const Token::Token& token, const std::string &message);
}

#endif //TOYPROJECTS_Interpreter_v1_ERROR_H
