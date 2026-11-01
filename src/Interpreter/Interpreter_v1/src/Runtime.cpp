//
// Created by YangKai.Lin on 2026/11/1.
//
#include "Runtime.h"

#include <chrono>

#include "AstInterpreter.h"

namespace CLOX_V1::Runtime {

    // ---------------------------------------------------------------- Instance

    std::any LoxInstance::get(const Token::Token &name) {
        auto it = fields_.find(name.lexeme);
        if (it != fields_.end()) return it->second;

        // 不是字段就看是不是方法。方法要先 bind 到当前实例，
        // 这样方法体里的 this 才指向正确的对象。
        if (auto method = klass_->findMethod(name.lexeme)) {
            return std::any{CallablePtr{method->bind(shared_from_this())}};
        }

        throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
    }

    void LoxInstance::set(const Token::Token &name, std::any value) {
        fields_.insert_or_assign(name.lexeme, std::move(value));
    }

    std::string LoxInstance::toString() const {
        return klass_->name() + " instance";
    }

    // ---------------------------------------------------------------- Function

    std::any LoxFunction::call(Ast::AstInterpreter &interpreter,
                               const std::vector<std::any> &arguments) {
        // 每次调用一层新环境，参数作为这一层的局部变量定义进去。
        auto env = std::make_shared<Env::Environment>(closure_);
        for (std::size_t i = 0; i < declaration_->params.size(); ++i) {
            env->define(declaration_->params[i].lexeme, arguments[i]);
        }

        try {
            interpreter.executeBlock(declaration_->body, env);
        } catch (const ReturnSignal &signal) {
            // 构造函数显式 return 时，返回的仍然是 this（Lox 的规定）
            if (isInitializer_) return closure_->getAt(0, "this");
            return signal.value;
        }

        // 构造函数没 return 时也要返回 this
        if (isInitializer_) return closure_->getAt(0, "this");
        return nullptr;
    }

    std::shared_ptr<LoxFunction> LoxFunction::bind(InstancePtr instance) const {
        auto env = std::make_shared<Env::Environment>(closure_);
        env->define("this", std::any{instance});
        return std::make_shared<LoxFunction>(declaration_, env, isInitializer_);
    }

    // ------------------------------------------------------------------- Class

    std::shared_ptr<LoxFunction> LoxClass::findMethod(const std::string &name) const {
        auto it = methods_.find(name);
        if (it != methods_.end()) return it->second;

        // 自己没定义就去父类找，一直往上。
        if (superclass_) return superclass_->findMethod(name);

        return nullptr;
    }

    std::size_t LoxClass::arity() const {
        // 有 init 就以它的参数个数为准，没有就是 0 参构造。
        if (auto initializer = findMethod("init")) return initializer->arity();
        return 0;
    }

    std::any LoxClass::call(Ast::AstInterpreter &interpreter,
                            const std::vector<std::any> &arguments) {
        auto instance = std::make_shared<LoxInstance>(shared_from_this());
        if (auto initializer = findMethod("init")) {
            initializer->bind(instance)->call(interpreter, arguments);
        }
        return std::any{InstancePtr{instance}};
    }

    // ------------------------------------------------------------------ Native

    std::any NativeClock::call(Ast::AstInterpreter &, const std::vector<std::any> &) {
        const auto now = std::chrono::system_clock::now().time_since_epoch();
        const double seconds =
                std::chrono::duration_cast<std::chrono::duration<double> >(now).count();
        return seconds;
    }
}
