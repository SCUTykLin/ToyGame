//
// Created by YangKai.Lin on 2026/10/29.
//

#ifndef TOYPROJECTS_Interpreter_v1_PARSER_H
#define TOYPROJECTS_Interpreter_v1_PARSER_H

#include "Token.h"
#include <list>

#include "AstPrint.h"

namespace CLOX_V1::Parser {
    class Parser {
    public:
        Parser(std::list<Token::Token> tokens) : tokens(tokens) {
        };

    private:
        std::unique_ptr<Ast::Expr> expression() {
            return equality();
        }

        std::unique_ptr<Ast::Expr> equality() {
            std::unique_ptr<Ast::Expr> expr = comparison();
            while (match({Token::BANG_EQUAL, Token::EQUAL_EQUAL})) {
                Token::Token operator_ = previous();
                std::unique_ptr<Ast::Expr> right = comparison();
                expr = std::make_unique<Ast::Expr>(expr, operator_, right);
            }
            return std::move(expr);
        }

        bool Token::Token advance() {
            if (!isAtEnd()) current++;
            return previous();
        }

        bool match(std::initializer_list<Token::TokenType_t> types) {
            for (auto type: types) {
                if (check(type)) {
                    advance();
                    return true;
                }
            }
            return false;
        }

        bool check(Token::TokenType_t type) {
            if (isAtEnd()) return false;
            return peek().type == type;
        }
        std::list<Token::Token> tokens;
        int current = 0;
    };
}

#endif //TOYPROJECTS_Interpreter_v1_PARSER_H
