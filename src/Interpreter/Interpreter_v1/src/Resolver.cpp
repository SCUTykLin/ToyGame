//
// Created by YangKai.Lin on 2026/11/1.
//
#include "Resolver.h"

#include <string>

#include "error.h"

namespace CLOX_V1::Resolve {
    void Resolver::resolve(const std::vector<std::unique_ptr<Ast::Stmt> > &statements) {
        for (const auto &stmt: statements) {
            resolveStmt(stmt.get());
        }
    }

    void Resolver::error(const Token::Token &token, const std::string &message) {
        Err::error(token, message);
        hadError = true;
    }

    void Resolver::declare(const Token::Token &name) {
        if (scopes.empty()) return;   // 全局变量不做静态绑定
        scopes.back().insert_or_assign(name.lexeme, false);
    }

    void Resolver::define(const Token::Token &name) {
        if (scopes.empty()) return;
        scopes.back().insert_or_assign(name.lexeme, true);
    }

    void Resolver::resolveLocal(Ast::Expr *expr, const Token::Token &name) {
        for (int i = static_cast<int>(scopes.size()) - 1; i >= 0; --i) {
            if (scopes[static_cast<std::size_t>(i)].count(name.lexeme)) {
                const auto distance = scopes.size() - 1 - static_cast<std::size_t>(i);
                interpreter_.resolve(expr, distance);
                return;
            }
        }
        // 找不到说明是全局变量，求值时走 globals
    }

    void Resolver::resolveFunction(Ast::Function *function, FunctionType type) {
        const FunctionType enclosing = currentFunction;
        currentFunction = type;

        beginScope();
        for (const auto &param: function->params) {
            declare(param);
            define(param);
        }
        for (const auto &stmt: function->body) {
            resolveStmt(stmt.get());
        }
        endScope();

        currentFunction = enclosing;
    }

    // ---------------------------------------------------------------- 表达式

    std::any Resolver::visitVariable(Ast::Variable *expr) {
        if (!scopes.empty()) {
            auto it = scopes.back().find(expr->name.lexeme);
            if (it != scopes.back().end() && !it->second) {
                error(expr->name, "Can't read local variable in its own initializer.");
            }
        }
        resolveLocal(expr, expr->name);
        return nullptr;
    }

    std::any Resolver::visitAssign(Ast::Assign *expr) {
        resolveExpr(expr->value.get());
        resolveLocal(expr, expr->name);
        return nullptr;
    }

    std::any Resolver::visitBinary(Ast::Binary *expr) {
        resolveExpr(expr->left.get());
        resolveExpr(expr->right.get());
        return nullptr;
    }

    std::any Resolver::visitCall(Ast::Call *expr) {
        resolveExpr(expr->callee.get());
        for (const auto &arg: expr->arguments) resolveExpr(arg.get());
        return nullptr;
    }

    std::any Resolver::visitGet(Ast::Get *expr) {
        resolveExpr(expr->object.get());
        return nullptr;
    }

    std::any Resolver::visitGrouping(Ast::Grouping *expr) {
        resolveExpr(expr->expression.get());
        return nullptr;
    }

    std::any Resolver::visitLiteral(Ast::Literal *) {
        return nullptr;
    }

    std::any Resolver::visitLogical(Ast::Logical *expr) {
        resolveExpr(expr->left.get());
        resolveExpr(expr->right.get());
        return nullptr;
    }

    std::any Resolver::visitSet(Ast::Set *expr) {
        resolveExpr(expr->value.get());
        resolveExpr(expr->object.get());
        return nullptr;
    }

    std::any Resolver::visitSuper(Ast::Super *expr) {
        if (currentClass == ClassType::NONE) {
            error(expr->keyword, "Can't use 'super' outside of a class.");
        } else if (currentClass != ClassType::SUBCLASS) {
            error(expr->keyword, "Can't use 'super' in a class with no superclass.");
        }
        resolveLocal(expr, expr->keyword);
        return nullptr;
    }

    std::any Resolver::visitThis(Ast::This *expr) {
        if (currentClass == ClassType::NONE) {
            error(expr->keyword, "Can't use 'this' outside of a class.");
            return nullptr;
        }
        resolveLocal(expr, expr->keyword);
        return nullptr;
    }

    std::any Resolver::visitUnary(Ast::Unary *expr) {
        resolveExpr(expr->right.get());
        return nullptr;
    }

    // ---------------------------------------------------------------- 语句

    std::any Resolver::visitBlockStmt(Ast::Block *stmt) {
        beginScope();
        for (const auto &s: stmt->statements) resolveStmt(s.get());
        endScope();
        return nullptr;
    }

    std::any Resolver::visitClassStmt(Ast::Class *stmt) {
        const ClassType enclosing = currentClass;
        currentClass = ClassType::CLASS;

        declare(stmt->name);
        define(stmt->name);

        if (stmt->superclass) {
            if (stmt->name.lexeme == stmt->superclass->name.lexeme) {
                error(stmt->superclass->name, "A class can't inherit from itself.");
            }

            currentClass = ClassType::SUBCLASS;
            resolveExpr(stmt->superclass.get());

            // super 和 this 各占一层作用域，求值时按层数取
            beginScope();
            scopes.back().insert_or_assign("super", true);
        }

        beginScope();
        scopes.back().insert_or_assign("this", true);

        for (const auto &method: stmt->methods) {
            const FunctionType type =
                    method->name.lexeme == "init" ? FunctionType::INITIALIZER : FunctionType::METHOD;
            resolveFunction(method.get(), type);
        }

        endScope();
        if (stmt->superclass) endScope();

        currentClass = enclosing;
        return nullptr;
    }

    std::any Resolver::visitExpressionStmt(Ast::Expression *stmt) {
        resolveExpr(stmt->expression.get());
        return nullptr;
    }

    std::any Resolver::visitFunctionStmt(Ast::Function *stmt) {
        declare(stmt->name);
        define(stmt->name);
        resolveFunction(stmt, FunctionType::FUNCTION);
        return nullptr;
    }

    std::any Resolver::visitIfStmt(Ast::If *stmt) {
        resolveExpr(stmt->condition.get());
        resolveStmt(stmt->thenBranch.get());
        if (stmt->elseBranch) resolveStmt(stmt->elseBranch.get());
        return nullptr;
    }

    std::any Resolver::visitPrintStmt(Ast::Print *stmt) {
        resolveExpr(stmt->expression.get());
        return nullptr;
    }

    std::any Resolver::visitReturnStmt(Ast::Return *stmt) {
        if (currentFunction == FunctionType::NONE) {
            error(stmt->keyword, "Can't return from top-level code.");
        }
        if (stmt->value) {
            if (currentFunction == FunctionType::INITIALIZER) {
                error(stmt->keyword, "Can't return a value from an initializer.");
            }
            resolveExpr(stmt->value.get());
        }
        return nullptr;
    }

    std::any Resolver::visitVarStmt(Ast::Var *stmt) {
        declare(stmt->name);
        if (stmt->initializer) {
            resolveExpr(stmt->initializer.get());
        }
        define(stmt->name);
        return nullptr;
    }

    std::any Resolver::visitWhileStmt(Ast::While *stmt) {
        resolveExpr(stmt->condition.get());
        resolveStmt(stmt->body.get());
        return nullptr;
    }
}
