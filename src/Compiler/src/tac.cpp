//
// Created by 15813 on 2026/7/17.
//

#include "../include/tac.h"
#include "../include/lexer.h"
#include "../include/io.h"
#include "../include/parser.h"

namespace Compiler::tac {
    void tac_compile(char* src) {
        Lexer::lexer_T* lexer = Lexer::init_lexer(src);
        Parser::parser_T* parser = Parser::init_parser(lexer);
        AST::AST_T* root = Parser::parser_parse(parser);
        Token::token_T* token = nullptr;
        while ((token = Lexer::lexer_next_token(lexer))->type != Token::TOKEN_STRUCT::TOKEN_EOF) {
            printf("%s", Token::token_to_str(token));
        }
    }
    void tac_complie_file(const char* filename) {
        std::string src_str = IO::tac_read_file(filename);
        tac::tac_compile(src_str.data());
    }
}