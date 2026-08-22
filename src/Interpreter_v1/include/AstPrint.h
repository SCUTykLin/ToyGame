//
// Created by 15813 on 2026/8/22.
//

#ifndef TOYPROJECTS_Interpreter_v1_ASTPRINT_H
#define TOYPROJECTS_Interpreter_v1_ASTPRINT_H

#include "Expr.h"
#include <sstream>

namespace CLOX_V1::Ast{
    class AstPrinter : public Visitor{
    public:

        template<typename... Args>
        std::string parenthesize(const std::string& name, const Args& ... args){
            std::stringstream ss;
            ss << "(" << name;
            ((ss << std::any_cast<std::string>(args->accept(this))), ...);
            ss << ")";
            return ss.str();
        }

        std::string print(const std::unique_ptr<Expr>& expr){
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

        std::any visitLiteral(Literal* literal) override {
            if (literal->type != Token::TokenType_t::EOF_Type){
                if (literal->type == Token::NUMBER) return std::to_string(std::any_cast<double>(literal->value));
                else return std::any_cast<std::string>(literal->value);
            }
            return std::string("");
        }
    };
}

#endif //TOYPROJECTS_Interpreter_v1_ASTPRINT_H
