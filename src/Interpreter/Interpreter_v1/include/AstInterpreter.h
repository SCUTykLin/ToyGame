//
// Created by YangKai.Lin on 2026/11/1.
//

#ifndef TOYPROJECTS_Interpreter_v1_ASTINTERPRETER_H
#define TOYPROJECTS_Interpreter_v1_ASTINTERPRETER_H

#include <any>
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Environment.h"
#include "Expr.h"
#include "Runtime.h"
#include "RuntimeError.h"
#include "Stmt.h"
#include "Token.h"

namespace CLOX_V1::Ast {
    class AstInterpreter : public Visitor, public StmtVisitor {
    public:
        AstInterpreter();

        // 按值收：解释器要自己持有整棵 AST，因为 LoxFunction 里存的是指向
        // Stmt::Function 的裸指针。AST 必须活得比所有函数久。
        void interpret(std::vector<std::unique_ptr<Stmt> > statements);

        bool hasRuntimeError() const { return hadRuntimeError; }

        // 第 11 章：resolver 算出「这个变量隔几层」，存在这里。
        // 求值时按层数直接跳，不用再按名字从里到外查一遍。
        void resolve(const Expr *expr, std::size_t depth) { locals_[expr] = depth; }

        // LoxFunction::call 要调它执行函数体，所以是 public
        void executeBlock(const std::vector<std::unique_ptr<Stmt> > &statements,
                          Env::EnvironmentPtr env);

        // ---- 表达式 ----
        std::any visitAssign(Assign *assign) override;

        std::any visitBinary(Binary *binary) override;

        std::any visitCall(Call *call) override;

        std::any visitGet(Get *get) override;

        std::any visitGrouping(Grouping *grouping) override;

        std::any visitLiteral(Literal *literal) override;

        std::any visitLogical(Logical *logical) override;

        std::any visitSet(Set *set) override;

        std::any visitSuper(Super *super) override;

        std::any visitThis(This *thisExpr) override;

        std::any visitUnary(Unary *unary) override;

        std::any visitVariable(Variable *variable) override;

        // ---- 语句 ----
        std::any visitBlockStmt(Block *stmt) override;

        std::any visitClassStmt(Class *stmt) override;

        std::any visitExpressionStmt(Expression *stmt) override;

        std::any visitFunctionStmt(Function *stmt) override;

        std::any visitIfStmt(If *stmt) override;

        std::any visitPrintStmt(Print *stmt) override;

        std::any visitReturnStmt(Return *stmt) override;

        std::any visitVarStmt(Var *stmt) override;

        std::any visitWhileStmt(While *stmt) override;

        // 给 LoxFunction 用：拿到当前环境（bind 时要挂 this）
        const Env::EnvironmentPtr &currentEnvironment() const { return environment_; }

    private:
        std::vector<std::unique_ptr<Stmt> > ast_;
        Env::EnvironmentPtr globals_;
        Env::EnvironmentPtr environment_;
        std::unordered_map<const Expr *, std::size_t> locals_;
        bool hadRuntimeError = false;

        std::any evaluate(const std::unique_ptr<Expr> &expr);

        void execute(const std::unique_ptr<Stmt> &stmt) { stmt->accept(this); }

        std::any lookUpVariable(const Token::Token &name, const Expr *expr);

        static bool isNil(const std::any &value);

        static bool isTruthy(const std::any &value);

        static bool isEqual(const std::any &a, const std::any &b);

        static double checkNumberOperand(const Token::Token &op, const std::any &operand);

        static void checkNumberOperands(const Token::Token &op,
                                        const std::any &left,
                                        const std::any &right);

        static std::string stringify(const std::any &value);
    };
}

#endif //TOYPROJECTS_Interpreter_v1_ASTINTERPRETER_H
