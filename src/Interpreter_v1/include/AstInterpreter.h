//
// Created by YangKai.Lin on 2026/11/1.
//

#ifndef TOYPROJECTS_Interpreter_v1_ASTINTERPRETER_H
#define TOYPROJECTS_Interpreter_v1_ASTINTERPRETER_H

#include <any>
#include <memory>
#include <string>
#include "Stmt.h"
#include "Expr.h"
#include "RuntimeError.h"
#include "Token.h"

namespace CLOX_V1::Ast {
    class AstInterpreter : public Visitor, public StmtVisitor {
    public:
        void interpret(std::unique_ptr<Expr> expr);

        bool hasRuntimeError() const { return hadRuntimeError; }

        std::any visitLiteral(Literal *literal) override;

        std::any visitGrouping(Grouping *grouping) override;

        std::any visitBinary(Binary *binary) override;

        std::any visitUnary(Unary *unary) override;

        std::any visitBlockStmt(Block *stmt) override;

        std::any visitExpressionStmt(Expression *stmt) override;

        std::any visitPrintStmt(Print *stmt) override;

        std::any visitVarStmt(Var *stmt) override;

    private:
        bool hadRuntimeError = false;

        std::any evaluate(const std::unique_ptr<Expr> &expr);

        static bool isNil(const std::any &value);

        static bool isTruthy(const std::any &value);

        static bool isEqual(const std::any &a, const std::any &b);

        static double checkNumberOperand(const Token::Token &op, const std::any &operand);

        static void checkNumberOperands(const Token::Token &op, const std::any &left, const std::any &right);

        static std::string stringify(const std::any &value);
    };
}

#endif //TOYPROJECTS_Interpreter_v1_ASTINTERPRETER_H
