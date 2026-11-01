//
// Created by 15813 on 2026/8/21.
//

#ifndef TOYPROJECTS_Interpreter_v1_COMPILER_H
#define TOYPROJECTS_Interpreter_v1_COMPILER_H

#include <string>

namespace CLOX_V1::Compiler{
    class CompilerSystem{
    public:
        void run(const std::string& source_code);
        bool hasError() const {return hadError;}
        // 运行时错误和语法错误是两个独立的标志：
        // 前者说明代码执行到一半崩了（前面已经产生的输出有效），
        // 后者说明整段代码根本没执行。合成一个会让 REPL 一次性失效。
        bool hasRuntimeError() const {return hadRuntimeError;}
    private:
        bool hadError = false;
        bool hadRuntimeError = false;
    };
}

#endif //TOYPROJECTS_Interpreter_v1_COMPILER_H
