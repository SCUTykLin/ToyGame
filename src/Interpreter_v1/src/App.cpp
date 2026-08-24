//
// Created by 15813 on 2026/8/20.
//
#include "App.h"
#include <cassert>
#include "IO.h"
#include "Compiler.h"

/**
 *
 * Test Printer
 *
 * #include "Expr.h"
 * #include "AstPrint.h"
 * std::unique_ptr<Ast::Expr> expression = std::make_unique<Ast::Binary>(
 *          std::make_unique<Ast::Unary>(Token::Token(Token::MINUS, "-", "", 1), std::make_unique<Ast::Literal>(123.0, Token::NUMBER)),
 *          Token::Token(Token::STAR, "*", "", 1),
 *          std::make_unique<Ast::Grouping>(std::make_unique<Ast::Literal>(45.67, Token::NUMBER)));

 * std::cout << Ast::AstPrinter{}.print(expression) << std::endl;
 *
 */


namespace CLOX_V1 {
    const char* source_file = "src/Interpreter_v1/example/print.lox";
    void App::run() {
        if (!source_file) assert("No Source Code!");
        const std::string source_code = IO::IOSystem::readAllBytes(source_file);

        Compiler::CompilerSystem compilerSystem;
        compilerSystem.run(source_code);
        if (compilerSystem.hasError()) assert("Compile Source code failed.");
    }
}