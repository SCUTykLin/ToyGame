//
// Created by 15813 on 2026/8/21.
//
#include "Compiler.h"
#include <iostream>
#include <utility>
#include "Scanner.h"
#include "Parser.h"
#include "AstPrint.h"

namespace CLOX_V1::Compiler{
    void CompilerSystem::run(const std::string& source_code){
        Scanner::ScannerSystem scanner(source_code);
        auto tokens = scanner.scanTokens();
        if (scanner.hasError()) {
            hadError = true;
            return;
        }

        Parser::Parser parser(std::move(tokens));
        auto expression = parser.parse();
        if (parser.hasError()) {
            hadError = true;
            return;
        }

        std::cout << Ast::AstPrinter{}.print(expression) << std::endl;
    }
}
