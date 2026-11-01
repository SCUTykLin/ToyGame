//
// Created by YangKai.Lin on 2026/11/1.
//
// 运行时对象：所有「值」里那些不是数字/布尔/字符串/nil 的东西。
//
// 它们互相依赖得厉害（类要有方法、方法要能绑定实例、实例要能查方法），
// 拆成多个头文件会绕死在循环 include 上，所以放一起。
//

#ifndef TOYPROJECTS_Interpreter_v1_RUNTIME_H
#define TOYPROJECTS_Interpreter_v1_RUNTIME_H

#include <any>
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Environment.h"
#include "Stmt.h"
#include "Token.h"

namespace CLOX_V1::Ast {
    class AstInterpreter;
}

namespace CLOX_V1::Runtime {
    // ---------------------------------------------------------------- 调用协议

    // 能被「调用」的东西：普通函数、类（构造）、native 函数。
    // 书里叫 LoxCallable。
    class LoxCallable {
    public:
        virtual ~LoxCallable() = default;

        virtual std::size_t arity() const = 0;

        virtual std::any call(Ast::AstInterpreter &interpreter,
                              const std::vector<std::any> &arguments) = 0;

        virtual std::string toString() const = 0;
    };

    using CallablePtr = std::shared_ptr<LoxCallable>;

    // return 用异常做控制流：在一层层嵌套的 while / if 里遇到 return，
    // 必须一口气跳出所有执行中的语句。返回值随异常带走。
    // 它不是错误，所以不继承 std::exception。
    struct ReturnSignal {
        std::any value;
    };

    // ---------------------------------------------------------------- 实例

    class LoxClass;

    class LoxFunction;

    class LoxInstance : public std::enable_shared_from_this<LoxInstance> {
    public:
        explicit LoxInstance(std::shared_ptr<LoxClass> klass)
            : klass_(std::move(klass)) {
        }

        std::any get(const Token::Token &name);

        void set(const Token::Token &name, std::any value);

        std::string toString() const;

    private:
        std::shared_ptr<LoxClass> klass_;
        std::unordered_map<std::string, std::any> fields_;
    };

    using InstancePtr = std::shared_ptr<LoxInstance>;

    // ---------------------------------------------------------------- 函数

    class LoxFunction : public LoxCallable {
    public:
        // declaration 是非拥有的：AST 由 AstInterpreter 持有，活到解释器销毁，
        // 而所有 LoxFunction 的生命期都不会超过解释器，所以裸指针安全。
        LoxFunction(const Ast::Function *declaration,
                    Env::EnvironmentPtr closure,
                    bool isInitializer)
            : declaration_(declaration),
              closure_(std::move(closure)),
              isInitializer_(isInitializer) {
        }

        std::size_t arity() const override { return declaration_->params.size(); }

        std::any call(Ast::AstInterpreter &interpreter,
                      const std::vector<std::any> &arguments) override;

        std::string toString() const override { return "<fn " + declaration_->name.lexeme + ">"; }

        const Ast::Function *declaration() const { return declaration_; }

        // 方法被调用时要多一层作用域，里面绑 this。
        // 每次取方法都产生一个新的 LoxFunction，这样同一个方法在不同实例上互不干扰。
        std::shared_ptr<LoxFunction> bind(InstancePtr instance) const;

    private:
        const Ast::Function *declaration_;
        Env::EnvironmentPtr closure_;
        bool isInitializer_;
    };

    // ---------------------------------------------------------------- 类

    class LoxClass : public LoxCallable, public std::enable_shared_from_this<LoxClass> {
    public:
        LoxClass(std::string name,
                 std::shared_ptr<LoxClass> superclass,
                 std::unordered_map<std::string, std::shared_ptr<LoxFunction> > methods)
            : name_(std::move(name)),
              superclass_(std::move(superclass)),
              methods_(std::move(methods)) {
        }

        std::shared_ptr<LoxFunction> findMethod(const std::string &name) const;

        std::size_t arity() const override;

        std::any call(Ast::AstInterpreter &interpreter,
                      const std::vector<std::any> &arguments) override;

        std::string toString() const override { return name_; }

        const std::string &name() const { return name_; }

        const std::shared_ptr<LoxClass> &superclass() const { return superclass_; }

    private:
        std::string name_;
        std::shared_ptr<LoxClass> superclass_;
        std::unordered_map<std::string, std::shared_ptr<LoxFunction> > methods_;
    };

    using ClassPtr = std::shared_ptr<LoxClass>;

    // ---------------------------------------------------------------- native

    // clock()：书里唯一的 native 函数，用来做计时和性能测试。
    class NativeClock : public LoxCallable {
    public:
        std::size_t arity() const override { return 0; }

        std::any call(Ast::AstInterpreter &interpreter,
                      const std::vector<std::any> &arguments) override;

        std::string toString() const override { return "<native fn>"; }
    };

    inline ClassPtr asClass(const std::any &value) {
        if (auto p = std::any_cast<ClassPtr>(&value)) return *p;
        return nullptr;
    }

    inline InstancePtr asInstance(const std::any &value) {
        if (auto p = std::any_cast<InstancePtr>(&value)) return *p;
        return nullptr;
    }

    inline CallablePtr asCallable(const std::any &value) {
        // 类也是可调用的（调用它等于构造实例），但它在 std::any 里存的是
        // ClassPtr 而不是 CallablePtr，any_cast 要求类型完全一致，所以这里要分开试。
        if (auto p = std::any_cast<CallablePtr>(&value)) return *p;
        if (auto p = std::any_cast<ClassPtr>(&value)) {
            return std::static_pointer_cast<LoxCallable>(*p);
        }
        return nullptr;
    }
}

#endif //TOYPROJECTS_Interpreter_v1_RUNTIME_H
