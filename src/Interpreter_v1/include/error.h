//
// Created by 15813 on 2026/8/21.
//

#ifndef TOYPROJECTS_Interpreter_v1_ERROR_H
#define TOYPROJECTS_Interpreter_v1_ERROR_H

#include <iostream>
#include <format>
#include <string>

namespace CLOX_V1::Token {
    class Token;
}

namespace CLOX_V1::Err {
    // 定义在头文件里的函数必须 inline，否则每个 include 它的 .cpp
    // 都会生成一份符号，链接期报 multiple definition。
    inline void report(int line, const std::string &where, const std::string &message) {
        std::cout << std::format("[line {}] Error {}: {}", line, where, message) << std::endl;
    }

    inline void error(int line, const std::string &message) {
        report(line, "", message);
    }

    void error(const Token::Token& token, const std::string &message);
}

#endif //TOYPROJECTS_Interpreter_v1_ERROR_H
