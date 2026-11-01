//
// Created by YangKai.Lin on 2026/11/1.
//
#include "AstInterpreter.h"

#include <format>
#include <iostream>
#include <typeinfo>
#include <utility>

#include "error.h"

namespace CLOX_V1::Ast {

    void AstInterpreter::interpret(std::unique_ptr<Expr> expr) {
        try {
            std::cout << stringify(evaluate(expr)) << std::endl;
        } catch (const RuntimeError& e) {
            Err::runtimeError(e.token, e.what());
            hadRuntimeError = true;
        }
    }

    std::any AstInterpreter::evaluate(const std::unique_ptr<Expr>& expr) {
        return expr->accept(this);
    }

    bool AstInterpreter::isNil(const std::any& value) {
        return value.type() == typeid(std::nullptr_t);
    }

    bool AstInterpreter::isTruthy(const std::any& value) {
        if (isNil(value)) return false;
        if (auto p = std::any_cast<bool>(&value)) return *p;
        return true;
    }

    bool AstInterpreter::isEqual(const std::any& a, const std::any& b) {
        // 先比类型，一行就处理掉「类型不同一律不等」和「都是 nil」两种情况。
        if (a.type() != b.type()) return false;

        if (a.type() == typeid(std::nullptr_t)) return true;
        if (auto pa = std::any_cast<double>(&a)) return *pa == std::any_cast<double>(b);
        if (auto pa = std::any_cast<bool>(&a)) return *pa == std::any_cast<bool>(b);
        if (auto pa = std::any_cast<std::string>(&a)) {
            return *pa == *std::any_cast<std::string>(&b);   // 指针版本，避免拷贝字符串
        }
        return false;
    }

    double AstInterpreter::checkNumberOperand(const Token::Token& op, const std::any& operand) {
        if (auto p = std::any_cast<double>(&operand)) return *p;
        throw RuntimeError(op, "Operand must be a number.");
    }

    void AstInterpreter::checkNumberOperands(const Token::Token& op, const std::any& left, const std::any& right) {
        if (std::any_cast<double>(&left) && std::any_cast<double>(&right)) return;
        throw RuntimeError(op, "Operands must be numbers.");
    }

    std::string AstInterpreter::stringify(const std::any& value) {
        if (isNil(value)) return "nil";
        if (auto p = std::any_cast<bool>(&value)) return *p ? "true" : "false";
        if (auto p = std::any_cast<double>(&value)) return std::format("{}", *p);
        if (auto p = std::any_cast<std::string>(&value)) return *p;
        return "<unknown>";
    }

    std::any AstInterpreter::visitLiteral(Literal *literal) {
        switch (literal->type) {
            case Token::NUMBER: return std::any_cast<double>(literal->value);
            case Token::STRING: return std::any_cast<std::string>(literal->value);
            case Token::TRUE:   return true;
            case Token::FALSE:  return false;
            case Token::NIL:    return nullptr;
            default:            return nullptr;
        }
    }

    std::any AstInterpreter::visitGrouping(Grouping *grouping) {
        return evaluate(grouping->expression);
    }

    std::any AstInterpreter::visitUnary(Unary *unary) {
        std::any right = evaluate(unary->right);

        switch (unary->operator_.type) {
            case Token::BANG:
                return !isTruthy(right);
            case Token::MINUS:
                return -checkNumberOperand(unary->operator_, right);
            default:
                break;
        }
        return nullptr;   // 不可达
    }

    std::any AstInterpreter::visitBlockStmt(Block *stmt) {
    }

    std::any AstInterpreter::visitExpressionStmt(Expression *stmt) {
    }

    std::any AstInterpreter::visitPrintStmt(Print *stmt) {
    }

    std::any AstInterpreter::visitVarStmt(Var *stmt) {
    }

    std::any AstInterpreter::visitBinary(Binary *binary) {
        std::any left = evaluate(binary->left);
        std::any right = evaluate(binary->right);

        switch (binary->op.type) {
            case Token::GREATER:
                checkNumberOperands(binary->op, left, right);
                return std::any_cast<double>(left) > std::any_cast<double>(right);
            case Token::GREATER_EQUAL:
                checkNumberOperands(binary->op, left, right);
                return std::any_cast<double>(left) >= std::any_cast<double>(right);
            case Token::LESS:
                checkNumberOperands(binary->op, left, right);
                return std::any_cast<double>(left) < std::any_cast<double>(right);
            case Token::LESS_EQUAL:
                checkNumberOperands(binary->op, left, right);
                return std::any_cast<double>(left) <= std::any_cast<double>(right);

            case Token::MINUS:
                checkNumberOperands(binary->op, left, right);
                return std::any_cast<double>(left) - std::any_cast<double>(right);
            case Token::SLASH:
                checkNumberOperands(binary->op, left, right);
                return std::any_cast<double>(left) / std::any_cast<double>(right);
            case Token::STAR:
                checkNumberOperands(binary->op, left, right);
                return std::any_cast<double>(left) * std::any_cast<double>(right);

            case Token::PLUS: {
                if (auto l = std::any_cast<double>(&left)) {
                    if (auto r = std::any_cast<double>(&right)) return *l + *r;
                }
                if (auto l = std::any_cast<std::string>(&left)) {
                    if (auto r = std::any_cast<std::string>(&right)) return *l + *r;
                }
                throw RuntimeError(binary->op, "Operands must be two numbers or two strings.");
            }

            case Token::BANG_EQUAL:   return !isEqual(left, right);
            case Token::EQUAL_EQUAL:  return isEqual(left, right);

            default:
                break;
        }
        return nullptr;   // 不可达
    }
}
