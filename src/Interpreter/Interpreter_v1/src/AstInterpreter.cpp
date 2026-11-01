//
// Created by YangKai.Lin on 2026/11/1.
//
#include "AstInterpreter.h"

#include <format>
#include <iostream>
#include <optional>
#include <string>
#include <typeinfo>
#include <utility>

#include "error.h"

namespace CLOX_V1::Ast {
    namespace {
        // RAII：构造时切进新环境，析构时自动恢复 —— 异常路径也会恢复。
        // 对应书里 Java 的 try / finally。C++ 没有 finally，所以绑到对象生命周期上。
        struct EnvironmentScope {
            Env::EnvironmentPtr *slot;
            Env::EnvironmentPtr saved;

            EnvironmentScope(Env::EnvironmentPtr *slot, Env::EnvironmentPtr env)
                : slot(slot), saved(*slot) {
                *slot = std::move(env);
            }

            ~EnvironmentScope() { *slot = saved; }

            EnvironmentScope(const EnvironmentScope &) = delete;

            EnvironmentScope &operator=(const EnvironmentScope &) = delete;
        };
    }

    AstInterpreter::AstInterpreter()
        : globals_(std::make_shared<Env::Environment>()), environment_(globals_) {
        // 书里唯一一个 native 函数
        globals_->define("clock", std::any{Runtime::CallablePtr{
                std::make_shared<Runtime::NativeClock>()}});
    }

    void AstInterpreter::interpret(std::vector<std::unique_ptr<Stmt> > statements) {
        ast_ = std::move(statements);   // 自己持有 AST，函数里的裸指针才安全

        try {
            for (const auto &statement: ast_) {
                execute(statement);
            }
        } catch (const RuntimeError &e) {
            Err::runtimeError(e.token, e.what());
            hadRuntimeError = true;
        } catch (const Runtime::ReturnSignal &) {
            // resolver 会拦下顶层 return，走到这里说明有 bug
            std::cerr << "Can't return from top-level code." << std::endl;
            hadRuntimeError = true;
        }
    }

    std::any AstInterpreter::evaluate(const std::unique_ptr<Expr> &expr) {
        return expr->accept(this);
    }

    void AstInterpreter::executeBlock(const std::vector<std::unique_ptr<Stmt> > &statements,
                                      Env::EnvironmentPtr env) {
        EnvironmentScope scope(&environment_, std::move(env));
        for (const auto &stmt: statements) {
            execute(stmt);
        }
    }

    // ------------------------------------------------------------ 变量查找

    std::any AstInterpreter::lookUpVariable(const Token::Token &name, const Expr *expr) {
        auto it = locals_.find(expr);
        if (it != locals_.end()) {
            return environment_->getAt(it->second, name.lexeme);
        }
        return globals_->get(name);
    }

    std::any AstInterpreter::visitVariable(Variable *variable) {
        return lookUpVariable(variable->name, variable);
    }

    std::any AstInterpreter::visitAssign(Assign *assign) {
        std::any value = evaluate(assign->value);

        auto it = locals_.find(assign);
        if (it != locals_.end()) {
            environment_->assignAt(it->second, assign->name, value);
        } else {
            globals_->assign(assign->name, value);
        }
        return value;
    }

    // -------------------------------------------------------------- 值语义

    bool AstInterpreter::isNil(const std::any &value) {
        return value.type() == typeid(std::nullptr_t);
    }

    bool AstInterpreter::isTruthy(const std::any &value) {
        if (isNil(value)) return false;
        if (auto p = std::any_cast<bool>(&value)) return *p;
        return true;   // 0、""、对象都为真
    }

    bool AstInterpreter::isEqual(const std::any &a, const std::any &b) {
        if (a.type() != b.type()) return false;

        if (a.type() == typeid(std::nullptr_t)) return true;
        if (auto pa = std::any_cast<double>(&a)) return *pa == std::any_cast<double>(b);
        if (auto pa = std::any_cast<bool>(&a)) return *pa == std::any_cast<bool>(b);
        if (auto pa = std::any_cast<std::string>(&a)) {
            return *pa == *std::any_cast<std::string>(&b);
        }
        // 函数 / 类 / 实例按「是不是同一个对象」比
        if (auto pa = std::any_cast<Runtime::CallablePtr>(&a)) {
            return *pa == std::any_cast<Runtime::CallablePtr>(b);
        }
        if (auto pa = std::any_cast<Runtime::InstancePtr>(&a)) {
            return *pa == std::any_cast<Runtime::InstancePtr>(b);
        }
        return false;
    }

    double AstInterpreter::checkNumberOperand(const Token::Token &op, const std::any &operand) {
        if (auto p = std::any_cast<double>(&operand)) return *p;
        throw RuntimeError(op, "Operand must be a number.");
    }

    void AstInterpreter::checkNumberOperands(const Token::Token &op,
                                             const std::any &left,
                                             const std::any &right) {
        if (std::any_cast<double>(&left) && std::any_cast<double>(&right)) return;
        throw RuntimeError(op, "Operands must be numbers.");
    }

    std::string AstInterpreter::stringify(const std::any &value) {
        if (isNil(value)) return "nil";
        if (auto p = std::any_cast<bool>(&value)) return *p ? "true" : "false";
        if (auto p = std::any_cast<double>(&value)) return std::format("{}", *p);
        if (auto p = std::any_cast<std::string>(&value)) return *p;
        if (auto c = Runtime::asCallable(value)) return c->toString();
        if (auto i = Runtime::asInstance(value)) return i->toString();
        return "<unknown>";
    }

    // ---------------------------------------------------------------- 表达式

    std::any AstInterpreter::visitLiteral(Literal *literal) {
        switch (literal->type) {
            case Token::NUMBER: return std::any_cast<double>(literal->value);
            case Token::STRING: return std::any_cast<std::string>(literal->value);
            case Token::TRUE: return true;
            case Token::FALSE: return false;
            case Token::NIL: return nullptr;
            default: return nullptr;
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
        return nullptr;
    }

    std::any AstInterpreter::visitLogical(Logical *logical) {
        std::any left = evaluate(logical->left);

        if (logical->op.type == Token::OR) {
            if (isTruthy(left)) return left;
        } else {
            if (!isTruthy(left)) return left;
        }
        return evaluate(logical->right);
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

            case Token::BANG_EQUAL: return !isEqual(left, right);
            case Token::EQUAL_EQUAL: return isEqual(left, right);

            default:
                break;
        }
        return nullptr;
    }

    std::any AstInterpreter::visitCall(Call *call) {
        std::any callee = evaluate(call->callee);

        std::vector<std::any> arguments;
        arguments.reserve(call->arguments.size());
        for (const auto &arg: call->arguments) {
            arguments.push_back(evaluate(arg));
        }

        auto fn = Runtime::asCallable(callee);
        if (!fn) {
            throw RuntimeError(call->paren, "Can only call functions and classes.");
        }
        if (arguments.size() != fn->arity()) {
            throw RuntimeError(call->paren,
                               "Expected " + std::to_string(fn->arity()) +
                               " arguments but got " + std::to_string(arguments.size()) + ".");
        }
        return fn->call(*this, arguments);
    }

    std::any AstInterpreter::visitGet(Get *get) {
        std::any object = evaluate(get->object);
        if (auto instance = Runtime::asInstance(object)) {
            return instance->get(get->name);
        }
        throw RuntimeError(get->name, "Only instances have properties.");
    }

    std::any AstInterpreter::visitSet(Set *set) {
        std::any object = evaluate(set->object);
        auto instance = Runtime::asInstance(object);
        if (!instance) {
            throw RuntimeError(set->name, "Only instances have fields.");
        }

        std::any value = evaluate(set->value);
        instance->set(set->name, value);
        return value;
    }

    std::any AstInterpreter::visitThis(This *thisExpr) {
        return lookUpVariable(thisExpr->keyword, thisExpr);
    }

    std::any AstInterpreter::visitSuper(Super *super) {
        const std::size_t distance = locals_.at(super);

        auto superclass = Runtime::asClass(environment_->getAt(distance, "super"));
        // this 在 super 外面那一层
        auto object = Runtime::asInstance(environment_->getAt(distance - 1, "this"));

        auto method = superclass->findMethod(super->method.lexeme);
        if (!method) {
            throw RuntimeError(super->method,
                               "Undefined property '" + super->method.lexeme + "'.");
        }
        return std::any{Runtime::CallablePtr{method->bind(object)}};
    }

    // ---------------------------------------------------------------- 语句

    std::any AstInterpreter::visitBlockStmt(Block *stmt) {
        executeBlock(stmt->statements, std::make_shared<Env::Environment>(environment_));
        return nullptr;
    }

    std::any AstInterpreter::visitClassStmt(Class *stmt) {
        std::shared_ptr<Runtime::LoxClass> superclass;
        if (stmt->superclass) {
            // 直接 accept，不要包一层临时 unique_ptr（那会在析构时把 AST 节点删掉）
            superclass = Runtime::asClass(stmt->superclass->accept(this));
            if (!superclass) {
                throw RuntimeError(stmt->superclass->name, "Superclass must be a class.");
            }
        }

        // 先定义成 nil，这样类的方法体里可以引用类自己
        environment_->define(stmt->name.lexeme, std::any{nullptr});

        // 有父类就多开一层环境放 super，方法闭包捕获它
        std::optional<EnvironmentScope> superScope;
        if (superclass) {
            superScope.emplace(&environment_,
                               std::make_shared<Env::Environment>(environment_));
            environment_->define("super", std::any{superclass});
        }

        std::unordered_map<std::string, std::shared_ptr<Runtime::LoxFunction> > methods;
        for (const auto &method: stmt->methods) {
            const bool isInitializer = method->name.lexeme == "init";
            auto fn = std::make_shared<Runtime::LoxFunction>(method.get(), environment_, isInitializer);
            methods.insert_or_assign(method->name.lexeme, fn);
        }

        auto klass = std::make_shared<Runtime::LoxClass>(stmt->name.lexeme, superclass,
                                                         std::move(methods));

        superScope.reset();   // 退出 super 那一层

        environment_->assign(stmt->name, std::any{Runtime::ClassPtr{klass}});
        return nullptr;
    }

    std::any AstInterpreter::visitExpressionStmt(Expression *stmt) {
        evaluate(stmt->expression);
        return nullptr;
    }

    std::any AstInterpreter::visitFunctionStmt(Function *stmt) {
        auto fn = std::make_shared<Runtime::LoxFunction>(stmt, environment_, false);
        environment_->define(stmt->name.lexeme, std::any{Runtime::CallablePtr{fn}});
        return nullptr;
    }

    std::any AstInterpreter::visitIfStmt(If *stmt) {
        if (isTruthy(evaluate(stmt->condition))) {
            execute(stmt->thenBranch);
        } else if (stmt->elseBranch) {
            execute(stmt->elseBranch);
        }
        return nullptr;
    }

    std::any AstInterpreter::visitPrintStmt(Print *stmt) {
        std::any value = evaluate(stmt->expression);
        std::cout << stringify(value) << std::endl;
        return nullptr;
    }

    std::any AstInterpreter::visitReturnStmt(Return *stmt) {
        std::any value = nullptr;
        if (stmt->value) {
            value = evaluate(stmt->value);
        }
        // 用异常做控制流：一口气跳出所有嵌套中的语句
        throw Runtime::ReturnSignal{value};
    }

    std::any AstInterpreter::visitVarStmt(Var *stmt) {
        std::any value = nullptr;
        if (stmt->initializer) {
            value = evaluate(stmt->initializer);
        }
        environment_->define(stmt->name.lexeme, value);
        return nullptr;
    }

    std::any AstInterpreter::visitWhileStmt(While *stmt) {
        while (isTruthy(evaluate(stmt->condition))) {
            execute(stmt->body);
        }
        return nullptr;
    }
}
