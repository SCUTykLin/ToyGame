//
// Created by YangKai.Lin on 2026/11/1.
//

/**
 * Stmt是语句的意思，Block: {...}
 * Expression: 表达式
 * Print: 输出表达式
 * Var: 变量
 */

#ifndef TOYPROJECTS_Interpreter_v1_STMT_H
#define TOYPROJECTS_Interpreter_v1_STMT_H

#include <any>
#include <memory>
#include <utility>
#include <vector>

#include "Expr.h"
#include "Token.h"

namespace CLOX_V1::Ast {
    class Block;
    class Expression;
    class Print;
    class Var;

    class StmtVisitor {
    public:
        virtual std::any visitBlockStmt(Block *stmt) = 0;

        virtual std::any visitExpressionStmt(Expression *stmt) = 0;

        virtual std::any visitPrintStmt(Print *stmt) = 0;

        virtual std::any visitVarStmt(Var *stmt) = 0;

        virtual ~StmtVisitor() = default;
    };

    class Stmt {
    public:
        virtual std::any accept(StmtVisitor *visitor) = 0;

        virtual ~Stmt() = default;
    };

    // { statement* }
    class Block : public Stmt {
    public:
        explicit Block(std::vector<std::unique_ptr<Stmt> > statements)
            : statements(std::move(statements)) {
        }

        std::any accept(StmtVisitor *visitor) override;

        const std::vector<std::unique_ptr<Stmt> > statements;
    };

    class Expression : public Stmt {
    public:
        explicit Expression(std::unique_ptr<Expr> expression)
            : expression(std::move(expression)) {
        }

        std::any accept(StmtVisitor *visitor) override;

        const std::unique_ptr<Expr> expression;
    };

    // "print" expression ";"
    class Print : public Stmt {
    public:
        explicit Print(std::unique_ptr<Expr> expression)
            : expression(std::move(expression)) {
        }

        std::any accept(StmtVisitor *visitor) override;

        const std::unique_ptr<Expr> expression;
    };

    class Var : public Stmt {
    public:
        Var(Token::Token name, std::unique_ptr<Expr> initializer)
            : name(std::move(name)), initializer(std::move(initializer)) {
        }

        std::any accept(StmtVisitor *visitor) override;

        const Token::Token name;
        const std::unique_ptr<Expr> initializer;
    };
}

#endif //TOYPROJECTS_Interpreter_v1_STMT_H
