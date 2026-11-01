//
// Created by 15813 on 2026/8/22.
//

#ifndef TOYPROJECTS_Interpreter_v1_ASTPRINT_H
#define TOYPROJECTS_Interpreter_v1_ASTPRINT_H

#include "Expr.h"
#include <format>
#include <sstream>
#include <string>

namespace CLOX_V1::Ast{
    class AstPrinter : public Visitor{
    public:

        template<typename... Args>
        std::string parenthesize(const std::string& name, const Args& ... args){
            std::stringstream ss;
            ss << "(" << name;
            ((ss << " " << std::any_cast<std::string>(args->accept(this))), ...);
            ss << ")";
            return ss.str();
        }

        std::string print(const std::unique_ptr<Expr>& expr){
            if (!expr) return std::string("<null>");
            return std::any_cast<std::string>(expr->accept(this));
        }

        std::any visitBinary(Binary* binary) override {
            return parenthesize(binary->op.lexeme, binary->left, binary->right);
        }

        std::any visitGrouping(Grouping* grouping) override {
            return parenthesize("group", grouping->expression);
        }

        std::any visitUnary(Unary* unary) override {
            return parenthesize(unary->operator_.lexeme, unary->right);
        }

        // Literal 的 value 是 std::any，光看它本身无法判断装的是什么，
        // 所以靠构造时打上的 token 类型来分派。nil / true / false 都必须单独处理，
        // 否则 any_cast<std::string> 会在运行时抛 bad_any_cast。
        std::any visitLiteral(Literal* literal) override {
            switch (literal->type) {
                case Token::NUMBER:
                    return std::format("{}", std::any_cast<double>(literal->value));
                case Token::STRING:
                    return std::any_cast<std::string>(literal->value);
                case Token::TRUE:
                    return std::string("true");
                case Token::FALSE:
                    return std::string("false");
                case Token::NIL:
                    return std::string("nil");
                default:
                    return std::string("");
            }
        }
    };
}

#endif //TOYPROJECTS_Interpreter_v1_ASTPRINT_H
