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
        return parser_parse_compound(parser);
    }

    AST::AST_T* Parser::parser_parse_list(Parser::parser_T* parser) {
        Parser::parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_LPAREN);
        AST::AST_T* compound = AST::init_ast(AST::AST_STRUCT::AST_COMPOUND);
        list::list_push(compound->children, parser_parse_expr(parser));
        while (parser->token->type == Token::TOKEN_STRUCT::TOKEN_COMMA) {
            Parser::parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_COMMA);
            list::list_push(compound->children, parser_parse_expr(parser));
        }
        Parser::parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_RPAREN);
        return compound;
    }

    AST::AST_T* Parser::parser_parse_compound(Parser::parser_T* parser) {
        AST::AST_T* compound = AST::init_ast(AST::AST_STRUCT::AST_COMPOUND);
        while (parser->token->type != Token::TOKEN_STRUCT::TOKEN_EOF) {
            list::list_push(compound->children, parser_parse_expr(parser));
        }
        return compound;
    }

    AST::AST_T* Parser::parser_parse_id(Parser::parser_T* parser){
        char* value = static_cast<char*>(calloc(strlen(parser->token->value)+ 1, sizeof(char)));
        strcpy(value, parser->token->value);
        Parser::parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_ID);
        if (parser->token->type == Token::TOKEN_STRUCT::TOKEN_EQUALS){
            Parser::parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_EQUALS);
            AST::AST_T* ast = AST::init_ast(AST::AST_STRUCT::AST_ASSIGNMENT);
            ast->name = value;
            ast->value = Parser::parser_parse_expr(parser);
            return ast;
        }
        // parse variable
        AST::AST_T* ast = AST::init_ast(AST::AST_STRUCT::AST_VARIABLE);
        ast->name = value;
        printf("%s\n", value);
        return ast;
    }

    AST::AST_T* Parser::parser_parse_expr(Parser::parser_T* parser){
        switch (parser->token->type) {
            case Token::TOKEN_STRUCT::TOKEN_ID:
                return Parser::parser_parse_id(parser);
            case Token::TOKEN_STRUCT::TOKEN_LPAREN:
                return Parser::parser_parse_list(parser);
            default: {
                printf("[Parser]: Unexpected token '%s\n'", Token::token_to_str(parser->token));
                exit(1);
            }
        }
    }
}