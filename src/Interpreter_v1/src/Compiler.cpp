//
// Created by 15813 on 2026/8/21.
//
#include "Compiler.h"
#include <iostream>
#include <format>
#include "Scanner.h"

namespace CLOX_V1::Compiler{
    void CompilerSystem::run(const std::string& source_code){
        Scanner::ScannerSystem scanner(source_code);
        auto tokens = scanner.scanTokens();

        for (auto& t : tokens){
            std::cout << t.toString() << std::endl;
        }
    }
}