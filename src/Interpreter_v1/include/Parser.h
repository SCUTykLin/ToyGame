//
// Created by YangKai.Lin on 2026/10/29.
//

#ifndef TOYPROJECTS_Interpreter_v1_PARSER_H
#define TOYPROJECTS_Interpreter_v1_PARSER_H

#include <cstddef>
#include <exception>
#include <initializer_list>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Expr.h"
#include "Token.h"
#include "error.h"
#include "Stmt.h"

namespace CLOX_V1::Parser {
    class Parser {
    public:
        explicit Parser(std::vector<Token::Token> tokens) : tokens(std::move(tokens)) {}

        std::list<Ast::Stmt> parse() {
            std::list<Ast::Stmt>* statements;
            while (!isAtEnd()) {
                statements->push_back(statement());
            }
        }

        bool hasError() const { return hadError; }

    private:
        struct ParseError : std::exception {
            const char* what() const noexcept override { return "parse error"; }
        };

        std::vector<Token::Token> tokens;
        std::size_t current = 0;
        bool hadError = false;

        bool isAtEnd() {
            return peek().type == Token::EOF_Type;
        }

        const Token::Token& peek() {
            return tokens[current];
        }

        const Token::Token& previous() {
            return tokens[current - 1];
        }

        const Token::Token& advance() {
            if (!isAtEnd()) ++current;
            return previous();
        }

        bool check(Token::TokenType_t type) {
            if (isAtEnd()) return false;
            return peek().type == type;
        }

        bool match(std::initializer_list<Token::TokenType_t> types) {
            for (auto type : types) {
                if (check(type)) {
                    advance();
                    return true;
                }
            }
            return false;
        }

        ParseError error(const Token::Token& token, const std::string& message) {
            Err::error(token, message);
            hadError = true;
            return ParseError{};
        }

        const Token::Token& consume(Token::TokenType_t type, const std::string& message) {
            if (check(type)) return advance();
            throw error(peek(), message);
        }

        std::unique_ptr<Ast::Expr> expression() {
            return equality();
        }

        std::unique_ptr<Ast::Stmt> statement() {
            if (match({Token::PRINT})) return printStatement();
            return expressionStatement();
        }

        std::unique_ptr<Ast::Stmt> printStatement() {
            std::unique_ptr<Ast::Expr> value = expression();
            consume(Token::SEMICOLON, "Expect ';' after value");
            return std::make_unique<Ast::Print>(std::move(value));
        }

        std::unique_ptr<Ast::Stmt> expressionStatement() {
            std::unique_ptr<Ast::Expr> expr = expression();
            consume(Token::SEMICOLON, "Expect ';' after expression.");
            return std::make_unique<Ast::Expression>(std::move(expr));
        }

        std::unique_ptr<Ast::Expr> equality() {
            auto expr = comparison();
            while (match({Token::BANG_EQUAL, Token::EQUAL_EQUAL})) {
                Token::Token op = previous();
                auto right = comparison();
                expr = std::make_unique<Ast::Binary>(std::move(expr), std::move(op), std::move(right));
            }
            return expr;
        }

        std::unique_ptr<Ast::Expr> comparison() {
            auto expr = term();
            while (match({Token::GREATER, Token::GREATER_EQUAL, Token::LESS, Token::LESS_EQUAL})) {
                Token::Token op = previous();
                auto right = term();
                expr = std::make_unique<Ast::Binary>(std::move(expr), std::move(op), std::move(right));
            }
            return expr;
        }

        std::unique_ptr<Ast::Expr> term() {
            auto expr = factor();
            while (match({Token::MINUS, Token::PLUS})) {
                Token::Token op = previous();
                auto right = factor();
                expr = std::make_unique<Ast::Binary>(std::move(expr), std::move(op), std::move(right));
            }
            return expr;
        }

        std::unique_ptr<Ast::Expr> factor() {
            auto expr = unary();
            while (match({Token::SLASH, Token::STAR})) {
                Token::Token op = previous();
                auto right = unary();
                expr = std::make_unique<Ast::Binary>(std::move(expr), std::move(op), std::move(right));
            }
            return expr;
        }

        std::unique_ptr<Ast::Expr> unary() {
            if (match({Token::BANG, Token::MINUS})) {
                Token::Token op = previous();
                auto right = unary();
                return std::make_unique<Ast::Unary>(std::move(op), std::move(right));
            }
            return primary();
        }

        std::unique_ptr<Ast::Expr> primary() {
            if (match({Token::FALSE})) return std::make_unique<Ast::Literal>(false, Token::FALSE);
            if (match({Token::TRUE})) return std::make_unique<Ast::Literal>(true, Token::TRUE);
            if (match({Token::NIL})) return std::make_unique<Ast::Literal>(nullptr, Token::NIL);

            if (match({Token::NUMBER})) {
                return std::make_unique<Ast::Literal>(previous().literal, Token::NUMBER);
            }
            if (match({Token::STRING})) {
                return std::make_unique<Ast::Literal>(previous().literal, Token::STRING);
            }

            if (match({Token::LEFT_PAREN})) {
                auto expr = expression();
                consume(Token::RIGHT_PAREN, "Expect ')' after expression.");
                return std::make_unique<Ast::Grouping>(std::move(expr));
            }

            throw error(peek(), "Expect expression.");
        }

        void synchronize() {
            advance();
            while (!isAtEnd()) {
                if (previous().type == Token::SEMICOLON) return;

                switch (peek().type) {
                    case Token::CLASS:
                    case Token::FUN:
                    case Token::VAR:
                    case Token::FOR:
                    case Token::IF:
                    case Token::WHILE:
                    case Token::PRINT:
                    case Token::RETURN:
                        return;
                    default:
                        break;
                }
                advance();
            }
        }
    };
}

#endif //TOYPROJECTS_Interpreter_v1_PARSER_H
