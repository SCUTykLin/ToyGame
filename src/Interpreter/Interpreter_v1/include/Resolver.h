//
// Created by YangKai.Lin on 2026/11/1.
//
// 第 11 章：解析和绑定。
//
// 为什么需要它：jlox 求值时按名字从里到外查环境，会踩到「动态作用域」的坑 ——
// 函数里引用的变量取决于「调用时」的环境，而不是「定义时」的。
// Resolver 在运行前把每个变量引用到「隔几层」算出来存进解释器，
// 求值时按层数直接跳，语义就变成词法作用域了。
//
// 顺带它还负责一堆静态检查：变量不能在自己初始化式里读自己、
// return 不能在顶层、this/super 只能在类里用、类不能继承自己。
//

#ifndef TOYPROJECTS_Interpreter_v1_RESOLVER_H
#define TOYPROJECTS_Interpreter_v1_RESOLVER_H

#include <any>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "AstInterpreter.h"
#include "Expr.h"
#include "Stmt.h"
#include "Token.h"

namespace CLOX_V1::Resolve {
    class Resolver : public Ast::Visitor, public Ast::StmtVisitor {
    public:
        explicit Resolver(Ast::AstInterpreter &interpreter) : interpreter_(interpreter) {
        }

        void resolve(const std::vector<std::unique_ptr<Ast::Stmt> > &statements);

        bool hasError() const { return hadError; }

        // ---- 表达式 ----
        std::any visitAssign(Ast::Assign *expr) override;

        std::any visitBinary(Ast::Binary *expr) override;

        std::any visitCall(Ast::Call *expr) override;

        std::any visitGet(Ast::Get *expr) override;

        std::any visitGrouping(Ast::Grouping *expr) override;

        std::any visitLiteral(Ast::Literal *expr) override;

        std::any visitLogical(Ast::Logical *expr) override;

        std::any visitSet(Ast::Set *expr) override;

        std::any visitSuper(Ast::Super *expr) override;

        std::any visitThis(Ast::This *expr) override;

        std::any visitUnary(Ast::Unary *expr) override;

        std::any visitVariable(Ast::Variable *expr) override;

        // ---- 语句 ----
        std::any visitBlockStmt(Ast::Block *stmt) override;

        std::any visitClassStmt(Ast::Class *stmt) override;

        std::any visitExpressionStmt(Ast::Expression *stmt) override;

        std::any visitFunctionStmt(Ast::Function *stmt) override;

        std::any visitIfStmt(Ast::If *stmt) override;

        std::any visitPrintStmt(Ast::Print *stmt) override;

        std::any visitReturnStmt(Ast::Return *stmt) override;

        std::any visitVarStmt(Ast::Var *stmt) override;

        std::any visitWhileStmt(Ast::While *stmt) override;

    private:
        enum class FunctionType { NONE, FUNCTION, INITIALIZER, METHOD };

        enum class ClassType { NONE, CLASS, SUBCLASS };

        Ast::AstInterpreter &interpreter_;
        bool hadError = false;

        // 作用域栈：每个元素是一层作用域里的「名字 → 是否已定义完」。
        // false 表示「声明了但初始化式还没解析完」，用来抓 a = a; 这种自引用。
        std::vector<std::unordered_map<std::string, bool> > scopes;

        FunctionType currentFunction = FunctionType::NONE;
        ClassType currentClass = ClassType::NONE;

        void resolveStmt(Ast::Stmt *stmt) { stmt->accept(this); }

        void resolveExpr(Ast::Expr *expr) { expr->accept(this); }

        void resolveFunction(Ast::Function *function, FunctionType type);

        void beginScope() { scopes.emplace_back(); }

        void endScope() { scopes.pop_back(); }

        void declare(const Token::Token &name);

        void define(const Token::Token &name);

        void resolveLocal(Ast::Expr *expr, const Token::Token &name);

        void error(const Token::Token &token, const std::string &message);
    };
}

#endif //TOYPROJECTS_Interpreter_v1_RESOLVER_H
