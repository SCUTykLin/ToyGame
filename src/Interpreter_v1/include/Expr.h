//
// Created by 15813 on 2026/8/22.
//

#ifndef TOYPROJECTS_Interpreter_v1_EXPR_H
#define TOYPROJECTS_Interpreter_v1_EXPR_H

#include "Token.h"
#include <any>
#include <memory>

namespace CLOX_V1::Ast{

    class Binary;
    class Grouping;
    class Unary;
    class Literal;

    class Visitor {
    public:
        virtual std::any visitBinary(Binary* binary) = 0;
        virtual std::any visitGrouping(Grouping* grouping) = 0;
        virtual std::any visitUnary(Unary* unary) = 0;
        virtual std::any visitLiteral(Literal* literal) = 0;
        virtual ~Visitor() = default;
    };

    class Expr{
    public:
        virtual std::any accept(Visitor* visitor) = 0;
        virtual ~Expr() = default;
    };

    class Binary : public Expr{
    public:
        Binary(std::unique_ptr<Expr> left, Token::Token op, std::unique_ptr<Expr> right) : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

        std::any accept(Visitor* visitor) override;

        const std::unique_ptr<Expr> left;
        const Token::Token op;
        const std::unique_ptr<Expr> right;
    };

    class Grouping : public Expr{
    public:
        Grouping(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {}

        std::any accept(Visitor* visitor) override;

        const std::unique_ptr<Expr> expression;
    };

    class Literal : public Expr{
    public:
        Literal(std::any value, Token::TokenType_t type) : value(std::move(value)), type(type) {};

        std::any accept(Visitor* visitor) override;

        const std::any value;
        const Token::TokenType_t type;
    };

    class Unary : public Expr{
    public:
        Unary(Token::Token op, std::unique_ptr<Expr> right) : operator_(std::move(op)), right(std::move(right)) {}

        std::any accept(Visitor* visitor) override;

        const Token::Token operator_;
        const std::unique_ptr<Expr> right;
    };
}

#endif //TOYPROJECTS_Interpreter_v1_EXPR_H
