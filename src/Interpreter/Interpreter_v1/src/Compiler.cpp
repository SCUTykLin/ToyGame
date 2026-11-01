//
// Created by 15813 on 2026/8/21.
//
#include "Compiler.h"
#include <iostream>
#include <utility>
#include "Scanner.h"
#include "Parser.h"
#include "AstInterpreter.h"
#include "Resolver.h"

namespace CLOX_V1::Compiler{
    void CompilerSystem::run(const std::string& source_code){
        Scanner::ScannerSystem scanner(source_code);
        auto tokens = scanner.scanTokens();
        if (scanner.hasError()) {
            hadError = true;
            return;
        }

        Parser::Parser parser(std::move(tokens));
        auto statements = parser.parse();
        if (parser.hasError()) {
            hadError = true;
            return;
        }

        Ast::AstInterpreter interpreter;

        // 第 11 章：先跑一遍静态作用域分析，把每个变量的「隔几层」算出来
        Resolve::Resolver resolver(interpreter);
        resolver.resolve(statements);
        if (resolver.hasError()) {
            hadError = true;
            return;
        }

        interpreter.interpret(std::move(statements));
        if (interpreter.hasRuntimeError()) {
            hadRuntimeError = true;
        }
    }
}
