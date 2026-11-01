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
        explicit Parser(std::vector<Token::Token> tokens) : tokens(std::move(tokens)) {
        }

        std::vector<std::unique_ptr<Ast::Stmt> > parse() {
            std::vector<std::unique_ptr<Ast::Stmt> > statements;
            while (!isAtEnd()) {
                statements.push_back(declaration());
            }
            return statements;
        }

        bool hasError() const { return hadError; }

    private:
        struct ParseError : std::exception {
            const char *what() const noexcept override { return "parse error"; }
        };

        std::vector<Token::Token> tokens;
        std::size_t current = 0;
        bool hadError = false;

        bool isAtEnd() {
            return peek().type == Token::EOF_Type;
        }

        const Token::Token &peek() {
            return tokens[current];
        }

        const Token::Token &previous() {
            return tokens[current - 1];
        }

        const Token::Token &advance() {
            if (!isAtEnd()) ++current;
            return previous();
        }

        bool check(Token::TokenType_t type) {
            if (isAtEnd()) return false;
            return peek().type == type;
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

        ParseError error(const Token::Token &token, const std::string &message) {
            Err::error(token, message);
            hadError = true;
            return ParseError{};
        }

        const Token::Token &consume(Token::TokenType_t type, const std::string &message) {
            if (check(type)) return advance();
            throw error(peek(), message);
        }

        std::unique_ptr<Ast::Expr> expression() {
            return assignment();
        }

        std::unique_ptr<Ast::Expr> assignment() {
            auto expr = logicOr();

            if (match({Token::EQUAL})) {
                Token::Token equals = previous();
                auto value = assignment();

                if (auto var = dynamic_cast<Ast::Variable*>(expr.get())) {
                    return std::make_unique<Ast::Assign>(var->name, std::move(value));
                }

                // a.b = c ：左边解析成了 Get，改造成 Set
                if (auto get = dynamic_cast<Ast::Get*>(expr.get())) {
                    Token::Token name = get->name;
                    return std::make_unique<Ast::Set>(std::move(get->object), name, std::move(value));
                }

                error(equals, "Invalid assignment target.");
            }

            return expr;
        }

        std::unique_ptr<Ast::Stmt> declaration() {
            try {
                if (match({Token::CLASS})) return classDeclaration();
                if (match({Token::FUN})) return function("function");
                if (match({Token::VAR})) return varDeclaration();
                return statement();
            } catch (ParseError &error) {
                synchronize();
                return nullptr;
            }
        }

        // "class" IDENTIFIER ( "<" IDENTIFIER )? "{" function* "}"
        std::unique_ptr<Ast::Stmt> classDeclaration() {
            Token::Token name = consume(Token::IDENTIFIER, "Expect class name.");

            std::unique_ptr<Ast::Variable> superclass;
            if (match({Token::LESS})) {
                consume(Token::IDENTIFIER, "Expect superclass name.");
                superclass = std::make_unique<Ast::Variable>(previous());
            }

            consume(Token::LEFT_BRACE, "Expect '{' before class body.");

            std::vector<std::unique_ptr<Ast::Function> > methods;
            while (!check(Token::RIGHT_BRACE) && !isAtEnd()) {
                methods.push_back(function("method"));
            }

            consume(Token::RIGHT_BRACE, "Expect '}' after class body.");
            return std::make_unique<Ast::Class>(name, std::move(superclass), std::move(methods));
        }

        // function → IDENTIFIER "(" parameters? ")" block
        // 普通函数和方法共用这一个函数，只是报错信息里的 kind 不同
        std::unique_ptr<Ast::Function> function(const std::string &kind) {
            Token::Token name = consume(Token::IDENTIFIER, "Expect " + kind + " name.");

            consume(Token::LEFT_PAREN, "Expect '(' after " + kind + " name.");
            std::vector<Token::Token> params;
            if (!check(Token::RIGHT_PAREN)) {
                do {
                    if (params.size() >= 255) {
                        error(peek(), "Can't have more than 255 parameters.");
                    }
                    params.push_back(consume(Token::IDENTIFIER, "Expect parameter name."));
                } while (match({Token::COMMA}));
            }
            consume(Token::RIGHT_PAREN, "Expect ')' after parameters.");

            consume(Token::LEFT_BRACE, "Expect '{' before " + kind + " body.");
            auto body = blockBody();
            return std::make_unique<Ast::Function>(name, std::move(params), std::move(body));
        }

        std::unique_ptr<Ast::Stmt> statement() {
            if (match({Token::LEFT_BRACE})) return block();
            if (match({Token::FOR})) return forStatement();
            if (match({Token::IF})) return ifStatement();
            if (match({Token::PRINT})) return printStatement();
            if (match({Token::RETURN})) return returnStatement();
            if (match({Token::WHILE})) return whileStatement();
            return expressionStatement();
        }

        // "return" expression? ";"
        std::unique_ptr<Ast::Stmt> returnStatement() {
            Token::Token keyword = previous();
            std::unique_ptr<Ast::Expr> value;
            if (!check(Token::SEMICOLON)) {
                value = expression();
            }
            consume(Token::SEMICOLON, "Expect ';' after return value.");
            return std::make_unique<Ast::Return>(keyword, std::move(value));
        }

        // "for" "(" ( varDecl | exprStmt | ";" )
        //           expression? ";"
        //           expression? ")" statement
        //
        // 语法糖：不产生任何新 AST 节点，直接展开成 Block + While + Expression。
        // 展开的顺序很关键（书上原话是 "build it back up from the inside out"）：
        //   ① 先把 increment 挂到 body 末尾
        //   ② 再用 condition 把 body 包成 While
        //   ③ 最后用 initializer 把 While 包成 Block
        std::unique_ptr<Ast::Stmt> forStatement() {
            consume(Token::LEFT_PAREN, "Expect '(' after 'for'.");

            // ① 初始化子句
            std::unique_ptr<Ast::Stmt> initializer;
            if (match({Token::SEMICOLON})) {
                initializer = nullptr;                       // for (; ...)
            } else if (match({Token::VAR})) {
                initializer = varDeclaration();
            } else {
                initializer = expressionStatement();
            }

            // ② 条件子句
            std::unique_ptr<Ast::Expr> condition;
            if (!check(Token::SEMICOLON)) {
                condition = expression();
            }
            consume(Token::SEMICOLON, "Expect ';' after loop condition.");

            // ③ 增量子句
            std::unique_ptr<Ast::Expr> increment;
            if (!check(Token::RIGHT_PAREN)) {
                increment = expression();
            }
            consume(Token::RIGHT_PAREN, "Expect ')' after for clauses.");

            std::unique_ptr<Ast::Stmt> body = statement();

            // 把 increment 拼到 body 末尾：{ body; increment; }
            if (increment) {
                std::vector<std::unique_ptr<Ast::Stmt>> stmts;
                stmts.push_back(std::move(body));
                stmts.push_back(std::make_unique<Ast::Expression>(std::move(increment)));
                body = std::make_unique<Ast::Block>(std::move(stmts));
            }

            // for (;;) 的条件按"恒真"处理
            if (!condition) {
                condition = std::make_unique<Ast::Literal>(true, Token::TRUE);
            }
            body = std::make_unique<Ast::While>(std::move(condition), std::move(body));

            // 最外面再包一层块，让 for (var i = ...) 的 i 不外泄
            if (initializer) {
                std::vector<std::unique_ptr<Ast::Stmt>> stmts;
                stmts.push_back(std::move(initializer));
                stmts.push_back(std::move(body));
                body = std::make_unique<Ast::Block>(std::move(stmts));
            }

            return body;
        }

        std::unique_ptr<Ast::Stmt> block() {
            return std::make_unique<Ast::Block>(blockBody());
        }

        // 已经吃掉 '{' 之后，读到匹配的 '}' 为止。block() 和 function() 共用。
        std::vector<std::unique_ptr<Ast::Stmt> > blockBody() {
            std::vector<std::unique_ptr<Ast::Stmt> > statements;
            while (!check(Token::RIGHT_BRACE) && !isAtEnd()) {
                auto decl = declaration();
                if (decl) statements.push_back(std::move(decl));
            }
            consume(Token::RIGHT_BRACE, "Expect '}' after block.");
            return statements;
        }

        std::unique_ptr<Ast::Stmt> ifStatement() {
            consume(Token::LEFT_PAREN, "Expect '(' after 'if'.");
            std::unique_ptr<Ast::Expr> condition = expression();
            consume(Token::RIGHT_PAREN, "Expect ')' after if condition.");

            std::unique_ptr<Ast::Stmt> thenBranch = statement();
            std::unique_ptr<Ast::Stmt> elseBranch = nullptr;
            if (match({Token::ELSE})) {
                elseBranch = statement();
            }
            return std::make_unique<Ast::If>(std::move(condition),
                                             std::move(thenBranch),
                                             std::move(elseBranch));
        }

        std::unique_ptr<Ast::Stmt> whileStatement() {
            consume(Token::LEFT_PAREN, "Expect '(' after 'while'.");
            std::unique_ptr<Ast::Expr> condition = expression();
            consume(Token::RIGHT_PAREN, "Expect ')' after condition.");
            std::unique_ptr<Ast::Stmt> body = statement();
            return std::make_unique<Ast::While>(std::move(condition), std::move(body));
        }

        std::unique_ptr<Ast::Stmt> printStatement() {
            std::unique_ptr<Ast::Expr> value = expression();
            consume(Token::SEMICOLON, "Expect ';' after value");
            return std::make_unique<Ast::Print>(std::move(value));
        }

        std::unique_ptr<Ast::Stmt> varDeclaration() {
            Token::Token name = consume(Token::IDENTIFIER, "Expect variable name.");
            std::unique_ptr<Ast::Expr> initializer = nullptr;
            if (match({Token::EQUAL})) {
                initializer = expression();
            }
            consume(Token::SEMICOLON, "Expect ';' after variable declaration.");
            return std::make_unique<Ast::Var>(name, std::move(initializer));
        }

        std::unique_ptr<Ast::Stmt> expressionStatement() {
            std::unique_ptr<Ast::Expr> expr = expression();
            consume(Token::SEMICOLON, "Expect ';' after expression.");
            return std::make_unique<Ast::Expression>(std::move(expr));
        }

        // 这两层插在 assignment 和 equality 之间：
        //   assignment → logic_or
        //   logic_or   → logic_and ( "or" logic_and )*
        //   logic_and  → equality ( "and" equality )*
        // or 比 and 松，所以 or 在外层。和 C 一致。
        std::unique_ptr<Ast::Expr> logicOr() {
            auto expr = logicAnd();
            while (match({Token::OR})) {
                Token::Token op = previous();
                auto right = logicAnd();
                expr = std::make_unique<Ast::Logical>(std::move(expr), std::move(op), std::move(right));
            }
            return expr;
        }

        std::unique_ptr<Ast::Expr> logicAnd() {
            auto expr = equality();
            while (match({Token::AND})) {
                Token::Token op = previous();
                auto right = equality();
                expr = std::make_unique<Ast::Logical>(std::move(expr), std::move(op), std::move(right));
            }
            return expr;
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
            return call();
        }

        // call → primary ( "(" arguments? ")" | "." IDENTIFIER )*
        // 后缀可以无限叠加：f()()、a.b().c 都合法
        std::unique_ptr<Ast::Expr> call() {
            auto expr = primary();

            while (true) {
                if (match({Token::LEFT_PAREN})) {
                    expr = finishCall(std::move(expr));
                } else if (match({Token::DOT})) {
                    Token::Token name = consume(Token::IDENTIFIER, "Expect property name after '.'.");
                    expr = std::make_unique<Ast::Get>(std::move(expr), name);
                } else {
                    break;
                }
            }
            return expr;
        }

        std::unique_ptr<Ast::Expr> finishCall(std::unique_ptr<Ast::Expr> callee) {
            std::vector<std::unique_ptr<Ast::Expr> > arguments;
            if (!check(Token::RIGHT_PAREN)) {
                do {
                    if (arguments.size() >= 255) {
                        error(peek(), "Can't have more than 255 arguments.");
                    }
                    arguments.push_back(expression());
                } while (match({Token::COMMA}));
            }

            Token::Token paren = consume(Token::RIGHT_PAREN, "Expect ')' after arguments.");
            return std::make_unique<Ast::Call>(std::move(callee), paren, std::move(arguments));
        }

        // 所有字面量和分组表达式：
        // NUMBER，STRING，TRUE，FALSE，NIL
        std::unique_ptr<Ast::Expr> primary() {
            if (match({Token::FALSE})) return std::make_unique<Ast::Literal>(false, Token::FALSE);
            if (match({Token::TRUE})) return std::make_unique<Ast::Literal>(true, Token::TRUE);
            if (match({Token::NIL})) return std::make_unique<Ast::Literal>(nullptr, Token::NIL);

            if (match({Token::THIS})) return std::make_unique<Ast::This>(previous());

            if (match({Token::SUPER})) {
                Token::Token keyword = previous();
                consume(Token::DOT, "Expect '.' after 'super'.");
                Token::Token method = consume(Token::IDENTIFIER, "Expect superclass method name.");
                return std::make_unique<Ast::Super>(keyword, method);
            }

            if (match({Token::NUMBER})) {
                return std::make_unique<Ast::Literal>(previous().literal, Token::NUMBER);
            }
            if (match({Token::STRING})) {
                return std::make_unique<Ast::Literal>(previous().literal, Token::STRING);
            }
            if (match({Token::IDENTIFIER})) {
                return std::make_unique<Ast::Variable>(previous());
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
