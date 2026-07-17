//
// Created by YangKai.Lin on 2026/7/17.
//

#include "../include/parser.h"


namespace Compiler {
    Parser::parser_T* Parser::init_parser(Lexer::lexer_T* lexer) {
        parser_T* parser = static_cast<parser_T *>(calloc(1, sizeof(struct PARSER_STRUCT)));
        parser->lexer = lexer;
        parser->token = Lexer::lexer_next_token(lexer);
        return parser;
    }

    Token::token_T* Parser::parser_eat(Parser::parser_T* parser, Token::TOKEN_STRUCT::TokenType type) {
        if (parser->token->type != type) {
            printf("[Parser]: Unexpected token: '%s', was expecting: '%s'\n",
                Token::token_to_str(parser->token), Token::token_type_to_str(type));
            exit(1);
        }
        parser->token = Lexer::lexer_next_token(parser->lexer);
        return parser->token;
    }
    AST::AST_T* Parser::parser_parse(Parser::parser_T* parser) {
        return AST::init_ast(AST::AST_STRUCT::AST_NOOP);
    }

    AST::AST_T* Parser::parser_parse_compound(Parser::parser_T* parser) {
        // AST::AST_T* compound = AST::init_ast(AST::AST_STRUCT::AST_COMPUND);
        while (parser->token->type != Token::TOKEN_STRUCT::TOKEN_EOF) {
            // AST::AST_T* child =
            Parser::parser_parse(parser);
        }
        return AST::init_ast(AST::AST_STRUCT::AST_NOOP);
    }
}