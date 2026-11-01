//
// Created by 15813 on 2026/8/22.
//

#ifndef TOYPROJECTS_Interpreter_v1_EXPR_H
#define TOYPROJECTS_Interpreter_v1_EXPR_H

#include "Token.h"
#include <any>
#include <memory>
#include <utility>
#include <vector>

namespace CLOX_V1::Ast {
    class Assign;
    class Binary;
    class Call;
    class Get;
    class Grouping;
    class Literal;
    class Logical;
    class Set;
    class Super;
    class This;
    class Unary;
    class Variable;

    class Visitor {
    public:
        virtual std::any visitAssign(Assign *assign) = 0;

        virtual std::any visitBinary(Binary *binary) = 0;

        virtual std::any visitCall(Call *call) = 0;

        virtual std::any visitGet(Get *get) = 0;

        virtual std::any visitGrouping(Grouping *grouping) = 0;

        virtual std::any visitLiteral(Literal *literal) = 0;

        virtual std::any visitLogical(Logical *logical) = 0;

        virtual std::any visitSet(Set *set) = 0;

        virtual std::any visitSuper(Super *super) = 0;

        virtual std::any visitThis(This *thisExpr) = 0;

        virtual std::any visitUnary(Unary *unary) = 0;

        virtual std::any visitVariable(Variable *variable) = 0;

        virtual ~Visitor() = default;
    };

    class Expr {
    public:
        virtual std::any accept(Visitor *visitor) = 0;

        virtual ~Expr() = default;
    };

    // 赋值：  a = 1
    // 注意它是个表达式，不是语句 —— 所以 (a = 1) 能嵌在别的地方，且求值结果是赋进去的值
    class Assign : public Expr {
    public:
        Assign(Token::Token name, std::unique_ptr<Expr> value)
            : name(std::move(name)), value(std::move(value)) {
        }

        std::any accept(Visitor *visitor) override;

        const Token::Token name;
        const std::unique_ptr<Expr> value;
    };

    class Binary : public Expr {
    public:
        Binary(std::unique_ptr<Expr> left, Token::Token op, std::unique_ptr<Expr> right)
            : left(std::move(left)), op(std::move(op)), right(std::move(right)) {
        }

        std::any accept(Visitor *visitor) override;

        const std::unique_ptr<Expr> left;
        const Token::Token op;
        const std::unique_ptr<Expr> right;
    };

    // 函数调用：  callee ( arg1, arg2, ... )
    // paren 存右括号的位置，用来在参数个数不对时报出准确行号
    class Call : public Expr {
    public:
        Call(std::unique_ptr<Expr> callee,
             Token::Token paren,
             std::vector<std::unique_ptr<Expr>> arguments)
            : callee(std::move(callee)),
              paren(std::move(paren)),
              arguments(std::move(arguments)) {
        }

        std::any accept(Visitor *visitor) override;

        const std::unique_ptr<Expr> callee;
        const Token::Token paren;
        const std::vector<std::unique_ptr<Expr>> arguments;
    };

    // 读属性：  object.name
    // object 没有加 const：a.b = c 解析时要把 object 从 Get 里搬出来交给 Set，
    // const 成员没法 move 出去。其余节点仍然全 const。
    class Get : public Expr {
    public:
        Get(std::unique_ptr<Expr> object, Token::Token name)
            : object(std::move(object)), name(std::move(name)) {
        }

        std::any accept(Visitor *visitor) override;

        std::unique_ptr<Expr> object;
        const Token::Token name;
    };

    class Grouping : public Expr {
    public:
        Grouping(std::unique_ptr<Expr> expression) : expression(std::move(expression)) {
        }

        std::any accept(Visitor *visitor) override;

        const std::unique_ptr<Expr> expression;
    };

    class Literal : public Expr {
    public:
        Literal(std::any value, Token::TokenType_t type) : value(std::move(value)), type(type) {
        }

        std::any accept(Visitor *visitor) override;

        const std::any value;
        const Token::TokenType_t type;
    };

    // 逻辑运算：  a and b   /   a or b
    // 形状和 Binary 一样，但求值规则完全不同 —— 必须短路，右边可能根本不求值。
    class Logical : public Expr {
    public:
        Logical(std::unique_ptr<Expr> left, Token::Token op, std::unique_ptr<Expr> right)
            : left(std::move(left)), op(std::move(op)), right(std::move(right)) {
        }

        std::any accept(Visitor *visitor) override;

        const std::unique_ptr<Expr> left;
        const Token::Token op;
        const std::unique_ptr<Expr> right;
    };

    // 写属性：  object.name = value
    // 它是表达式，求值结果是赋进去的那个 value
    class Set : public Expr {
    public:
        Set(std::unique_ptr<Expr> object, Token::Token name, std::unique_ptr<Expr> value)
            : object(std::move(object)), name(std::move(name)), value(std::move(value)) {
        }

        std::any accept(Visitor *visitor) override;

        const std::unique_ptr<Expr> object;
        const Token::Token name;
        const std::unique_ptr<Expr> value;
    };

    // super.method
    // keyword 是 super 那个 token（报错用），method 是方法名
    class Super : public Expr {
    public:
        Super(Token::Token keyword, Token::Token method)
            : keyword(std::move(keyword)), method(std::move(method)) {
        }

        std::any accept(Visitor *visitor) override;

        const Token::Token keyword;
        const Token::Token method;
    };

    // this
    // 它其实是个变量，只是 resolver 会给它在作用域里绑一个特殊名字
    class This : public Expr {
    public:
        explicit This(Token::Token keyword) : keyword(std::move(keyword)) {
        }

        std::any accept(Visitor *visitor) override;

        const Token::Token keyword;
    };

    class Unary : public Expr {
    public:
        Unary(Token::Token op, std::unique_ptr<Expr> right)
            : operator_(std::move(op)), right(std::move(right)) {
        }

        std::any accept(Visitor *visitor) override;

        const Token::Token operator_;
        const std::unique_ptr<Expr> right;
    };

    // 读取一个变量：  a
    // 和 Stmt::Var（声明 var a = 1;）是两回事，别混。
    class Variable : public Expr {
    public:
        explicit Variable(Token::Token name) : name(std::move(name)) {
        }

        std::any accept(Visitor *visitor) override;

        const Token::Token name;
    };
}

#endif //TOYPROJECTS_Interpreter_v1_EXPR_H
