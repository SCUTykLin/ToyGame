//
// Created by 15813 on 2026/8/21.
//

#ifndef TOYPROJECTS_Interpreter_v1_SCANNER_H
#define TOYPROJECTS_Interpreter_v1_SCANNER_H
#include <string>
#include "Token.h"
#include <vector>
#include <map>

namespace CLOX_V1::Scanner{
    class ScannerSystem{
    public:
        std::string source;
        std::vector<Token::Token> tokens;
        int start = 0;
        int current = 0;
        int line = 1;

        ScannerSystem(std::string source);
        std::vector<Token::Token> scanTokens();
        bool isAtEnd();
        inline bool hasError() {return hadError;}

    private:
        void scanToken();
        char advance();
        void addToken(Token::TokenType_t type);
        void addToken(Token::TokenType_t type, const std::any literal);
        bool match(char expected);
        char peek();
        char peekNext();
        void string();
        bool isDigit(char c);
        void number();
        bool isAlpha(char c);
        bool isAlphaNumeric(char c);
        void identifier();
        void comment();

        bool hadError = false;
        static std::map<std::string, Token::TokenType_t> keywords;
    };
}

#endif //TOYPROJECTS_Interpreter_v1_SCANNER_H
