//
// Created by YangKai.Lin on 2026/10/29.
//

#ifndef TOYPROJECTS_Interpreter_v1_PARSER_H
#define TOYPROJECTS_Interpreter_v1_PARSER_H

#include <format>

#include "Token.h"
#include <list>

#include "AstPrint.h"

namespace CLOX_V1::Parser {
    class Parser {
    public:
        Parser(std::list<Token::Token> tokens) : tokens(tokens) {
        };

    private:
        std::list<Token::Token> tokens;
        int current = 0;

        std::unique_ptr<Ast::Expr> expression() {
            return equality();
        }

        bool check(Token::TokenType_t type) {
            if (isAtEnd()) return false;
            return peek().type == type;
        }

        Token::Token advance() {
            if (!isAtEnd()) current++;
            return previous();
        }

        bool isAtEnd() {
            return peek().type == Token::EOF_Type;
        }

        Token::Token peek() {
            return *std::next(tokens.begin(), current);
        }

        Token::Token previous() {
            return *std::next(tokens.begin(), current - 1);
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

        Token::Token consume(Token::TokenType_t type, std::string message) {
            if (check(type)) return advance();
            throw std::runtime_error(std::format("Error: {}", message));
        }

        std::unique_ptr<Ast::Expr> equality() {
            std::unique_ptr<Ast::Expr> expr = comparison();
            while (match({Token::BANG_EQUAL, Token::EQUAL_EQUAL})) {
                Token::Token operator_ = previous();
                std::unique_ptr<Ast::Expr> right = comparison();
                expr = std::make_unique<Ast::Binary>(expr, operator_, right);
            }
            return std::move(expr);
        }

        std::unique_ptr<Ast::Expr> comparison() {
            std::unique_ptr<Ast::Expr> expr = term();
            while (match({Token::GREATER, Token::GREATER_EQUAL, Token::LESS, Token::LESS_EQUAL})) {
                Token::Token operator_ = previous();
                std::unique_ptr<Ast::Expr> right = term();
                expr = std::make_unique<Ast::Expr>()->Binary(expr, operator_, right);
            }
            return std::move(expr);
        }

        std::unique_ptr<Ast::Expr> term() {
            std::unique_ptr<Ast::Expr> expr = factor();
            while (match({Token::MINUS, Token::PLUS})) {
                Token::Token operator_ = previous();
                std::unique_ptr<Ast::Expr> right = factor();
                expr = std::make_unique<Ast::Expr>()->Binary(expr, operator_, right);
            }
        }

        std::unique_ptr<Ast::Expr> factor() {
            std::unique_ptr<Ast::Expr> expr = unary();
            while (match({Token::SLASH, Token::STAR})) {
                Token::Token operator_ = previous();
                std::unique_ptr<Ast::Expr> right = unary();
                expr = std::make_unique<Ast::Binary>(expr, operator_, right);
            }
            return std::move(expr);
        }

        std::unique_ptr<Ast::Expr> unary() {
            if (match({Token::BANG, Token::MINUS})) {
                Token::Token operator_ = previous();
                std::unique_ptr<Ast::Expr> right = unary();
                return std::make_unique<Ast::Unary>(operator_, right);
            }
            return primary();
        }

        std::unique_ptr<Ast::Expr> primary() {
            if (match({Token::FALSE})) return std::make_unique<Ast::Literal>(false, Token::FALSE);
            if (match({Token::TRUE})) return std::make_unique<Ast::Literal>(true, Token::TRUE);
            if (match({Token::NIL})) return std::make_unique<Ast::Literal>(nullptr, Token::NIL);

            if (match({Token::NUMBER})) {
                return std::make_unique<Ast::Literal>(previous().literal, Token::NUMBER);
            }else if (match({Token::STRING})) {
                return std::make_unique<Ast::Literal>(previous().literal, Token::STRING);
            }
        }
    };
}

#endif //TOYPROJECTS_Interpreter_v1_PARSER_H
