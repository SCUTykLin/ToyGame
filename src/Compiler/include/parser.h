//
// Created by YangKai.Lin on 2026/7/17.
//

#ifndef TOYPROJECTS_COMPILER_PARSER_H
#define TOYPROJECTS_COMPILER_PARSER_H

#include "lexer.h"
#include "AST.h"

namespace Compiler {
    namespace Parser {
        typedef struct PARSER_STRUCT {
            Lexer::lexer_T* lexer;
            Token::token_T* token;
        }parser_T;
        Parser::parser_T* init_parser(Lexer::lexer_T* lexer);
        Token::token_T* parser_eat(parser_T* parser, Token::TOKEN_STRUCT::TokenType type);
        AST::AST_T* parser_parse(parser_T* parser);
        AST::AST_T* parser_parse_compound(parser_T* parser);
        AST::AST_T* parser_parse_expr(parser_T* parser);
        AST::AST_T* parser_parse_id(parser_T* parser);
        AST::AST_T* parser_parse_list(parser_T* parser);
        AST::AST_T* parser_parse_block(parser_T* parser);
        AST::AST_T* parser_parse_int(parser_T* parser);
    }
}

#endif //TOYPROJECTS_COMPILER_PARSER_H