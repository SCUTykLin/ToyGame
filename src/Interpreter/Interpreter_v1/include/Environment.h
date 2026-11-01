//
// Created by YangKai.Lin on 2026/11/1.
//

#ifndef TOYPROJECTS_ENVIRONMENT_H
#define TOYPROJECTS_ENVIRONMENT_H

#include <any>
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "RuntimeError.h"
#include "Token.h"

namespace CLOX_V1::Env {
    // 为什么从「值 + 裸指针」改成 shared_ptr：
    //   第 8 章的作用域严格嵌套，栈帧一退环境就死，裸指针够用。
    //   第 10 章闭包要求「环境活过函数返回」—— 函数对象要抓着定义时的那层环境，
    //   而函数可能被返回、被存进变量。这时环境必须上堆，且要有人管它的命。
    //
    // 已知代价：函数声明在自己的作用域里时，环境存着函数、函数存着环境，形成环，
    //   shared_ptr 的引用计数回不到 0，这块内存会一直留着。
    //   玩具解释器里可以接受；真实实现靠 tracing GC。书里的 Java 版就是靠 JVM 的 GC 免费解决的。
    class Environment {
    public:
        Environment() = default;

        explicit Environment(std::shared_ptr<Environment> enclosing)
            : enclosing_(std::move(enclosing)) {
        }

        void define(const std::string &name, std::any value) {
            values_.insert_or_assign(name, std::move(value));
        }

        [[nodiscard]] std::any get(const Token::Token &name) const {
            auto it = values_.find(name.lexeme);
            if (it != values_.end()) return it->second;

            // 本层没有，去外层找。整条链都没有才算未定义。
            if (enclosing_) return enclosing_->get(name);

            throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
        }

        void assign(const Token::Token &name, std::any value) {
            auto it = values_.find(name.lexeme);
            if (it != values_.end()) {
                it->second = std::move(value);
                return;
            }

            // 本层没有，去外层赋值。注意是 assign 不是 define ——
            // 赋值只能改动已存在的变量，不能凭空造一个新的。
            if (enclosing_) {
                enclosing_->assign(name, std::move(value));
                return;
            }

            throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
        }

        [[nodiscard]] std::shared_ptr<Environment> enclosing() const { return enclosing_; }

        // ---- 第 11 章之后：resolver 算出了「隔几层」，按层数直接跳，不用再按名字查 ----

        std::any getAt(std::size_t distance, const std::string &name) {
            auto *env = this;
            for (std::size_t i = 0; i < distance; ++i) env = env->enclosing_.get();

            auto it = env->values_.find(name);
            if (it == env->values_.end()) return nullptr;
            return it->second;
        }

        void assignAt(std::size_t distance, const Token::Token &name, std::any value) {
            auto *env = this;
            for (std::size_t i = 0; i < distance; ++i) env = env->enclosing_.get();
            env->values_.insert_or_assign(name.lexeme, std::move(value));
        }

    private:
        std::unordered_map<std::string, std::any> values_;
        std::shared_ptr<Environment> enclosing_;
    };

    using EnvironmentPtr = std::shared_ptr<Environment>;
}

#endif //TOYPROJECTS_ENVIRONMENT_H
