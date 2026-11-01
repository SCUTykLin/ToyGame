//
// Created by YangKai.Lin on 2026/11/1.
//

/**
 * Stmt 是语句的意思。
 * 语句「做一件事」，表达式「算出一个值」—— 两套独立的继承体系，互不继承。
 *
 * Block       { ... }
 * Class       class Name < Super { ... }
 * Expression  expr;
 * Function    fun name(params) { body }
 * If          if (c) a else b
 * Print       print expr;
 * Return      return expr;
 * Var         var name = expr;
 * While       while (c) body
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
    class Class;
    class Expression;
    class Function;
    class If;
    class Print;
    class Return;
    class Var;
    class While;

    class StmtVisitor {
    public:
        virtual std::any visitBlockStmt(Block *stmt) = 0;

        virtual std::any visitClassStmt(Class *stmt) = 0;

        virtual std::any visitExpressionStmt(Expression *stmt) = 0;

        virtual std::any visitFunctionStmt(Function *stmt) = 0;

        virtual std::any visitIfStmt(If *stmt) = 0;

        virtual std::any visitPrintStmt(Print *stmt) = 0;

        virtual std::any visitReturnStmt(Return *stmt) = 0;

        virtual std::any visitVarStmt(Var *stmt) = 0;

        virtual std::any visitWhileStmt(While *stmt) = 0;

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

    // "class" IDENTIFIER ( "<" IDENTIFIER )? "{" function* "}"
    // superclass 可以为 nullptr（没有继承）
    class Class : public Stmt {
    public:
        Class(Token::Token name,
              std::unique_ptr<Variable> superclass,
              std::vector<std::unique_ptr<Function> > methods)
            : name(std::move(name)),
              superclass(std::move(superclass)),
              methods(std::move(methods)) {
        }

        std::any accept(StmtVisitor *visitor) override;

        const Token::Token name;
        const std::unique_ptr<Variable> superclass;
        const std::vector<std::unique_ptr<Function> > methods;
    };

    class Expression : public Stmt {
    public:
        explicit Expression(std::unique_ptr<Expr> expression)
            : expression(std::move(expression)) {
        }

        std::any accept(StmtVisitor *visitor) override;

        const std::unique_ptr<Expr> expression;
    };

    // "fun" function ;
    // function → IDENTIFIER "(" parameters? ")" block
    // 注意：方法（class 里的 fun）复用同一个节点
    class Function : public Stmt {
    public:
        Function(Token::Token name,
                 std::vector<Token::Token> params,
                 std::vector<std::unique_ptr<Stmt> > body)
            : name(std::move(name)),
              params(std::move(params)),
              body(std::move(body)) {
        }

        std::any accept(StmtVisitor *visitor) override;

        const Token::Token name;
        const std::vector<Token::Token> params;
        const std::vector<std::unique_ptr<Stmt> > body;
    };

    // "if" "(" expression ")" statement ( "else" statement )?
    class If : public Stmt {
    public:
        If(std::unique_ptr<Expr> condition,
           std::unique_ptr<Stmt> thenBranch,
           std::unique_ptr<Stmt> elseBranch)
            : condition(std::move(condition)),
              thenBranch(std::move(thenBranch)),
              elseBranch(std::move(elseBranch)) {
        }

        std::any accept(StmtVisitor *visitor) override;

        const std::unique_ptr<Expr> condition;
        const std::unique_ptr<Stmt> thenBranch;
        const std::unique_ptr<Stmt> elseBranch;
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

    // "return" expression? ";"
    // value 可以为 nullptr，对应裸 return;
    class Return : public Stmt {
    public:
        Return(Token::Token keyword, std::unique_ptr<Expr> value)
            : keyword(std::move(keyword)), value(std::move(value)) {
        }

        std::any accept(StmtVisitor *visitor) override;

        const Token::Token keyword;
        const std::unique_ptr<Expr> value;
    };

    // "var" IDENTIFIER ( "=" expression )? ";"
    // initializer 可以为 nullptr，对应 var a; 这种只声明不赋值的写法
    class Var : public Stmt {
    public:
        Var(Token::Token name, std::unique_ptr<Expr> initializer)
            : name(std::move(name)), initializer(std::move(initializer)) {
        }

        std::any accept(StmtVisitor *visitor) override;

        const Token::Token name;
        const std::unique_ptr<Expr> initializer;
    };

    // "while" "(" expression ")" statement
    class While : public Stmt {
    public:
        While(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
            : condition(std::move(condition)), body(std::move(body)) {
        }

        std::any accept(StmtVisitor *visitor) override;

        const std::unique_ptr<Expr> condition;
        const std::unique_ptr<Stmt> body;
    };
}

#endif //TOYPROJECTS_Interpreter_v1_STMT_H
