//
// Created by YangKai.Lin on 2026/7/17.
//

#include "../include/parser.h"
#include "../include/types.h"

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

    AST::AST_T* Parser::parser_parse_block(Parser::parser_T* parser){
        parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_LBRACE);
        AST::AST_T* ast = AST::init_ast(AST::AST_STRUCT::AST_COMPOUND);
        while(parser->token->type != Token::TOKEN_STRUCT::TOKEN_RBRACE){
            list::list_push(ast->children, parser_parse_expr(parser));
        }
        parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_RBRACE);
        return ast;
    }

    AST::AST_T* Parser::parser_parse_list(Parser::parser_T* parser) {
        Parser::parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_LPAREN);
        AST::AST_T* ast = AST::init_ast(AST::AST_STRUCT::AST_COMPOUND);
        list::list_push(ast->children, parser_parse_expr(parser));
        while (parser->token->type == Token::TOKEN_STRUCT::TOKEN_COMMA) {
            Parser::parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_COMMA);
            list::list_push(ast->children, parser_parse_expr(parser));
        }
        Parser::parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_RPAREN);

        if (parser->token->type == Token::TOKEN_STRUCT::TOKEN_COLON){
            parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_COLON);
            while(parser->token->type == Token::TOKEN_STRUCT::TOKEN_ID) {
                ast->data_type = Types::typename_to_int(parser->token->value);
                parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_ID);
                if (parser->token->type == Token::TOKEN_STRUCT::TOKEN_LT){
                    parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_LT);
                    ast->data_type += Types::typename_to_int(parser->token->value);
                    parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_ID);
                    parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_GT);
                }
            }
        }
        if (parser->token->type == Token::TOKEN_STRUCT::TOKEN_ARROW_RIGHT){
            parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_ARROW_RIGHT);
            ast->type = AST::AST_STRUCT::AST_FUNCTION;
            ast->value = parser_parse_compound(parser);
        }

        return ast;
    }

    AST::AST_T* Parser::parser_parse_compound(Parser::parser_T* parser) {

        unsigned int should_close = 0;
        if (parser->token->type == Token::TOKEN_STRUCT::TOKEN_LBRACE){
            parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_LBRACE);
            should_close = 1;
        }

        AST::AST_T* compound = AST::init_ast(AST::AST_STRUCT::AST_COMPOUND);
        while (parser->token->type != Token::TOKEN_STRUCT::TOKEN_EOF && parser->token->type != Token::TOKEN_STRUCT::TOKEN_RBRACE) {
            list::list_push(compound->children, parser_parse_expr(parser));
            if (parser->token->type == Token::TOKEN_STRUCT::TOKEN_SEMI){
                parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_SEMI);
            }
        }
        if (should_close){
            parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_RBRACE);
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

        if (parser->token->type == Token::TOKEN_STRUCT::TOKEN_COLON){
            parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_COLON);
            while(parser->token->type == Token::TOKEN_STRUCT::TOKEN_ID) {
                ast->data_type = Types::typename_to_int(parser->token->value);
                parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_ID);
                if (parser->token->type == Token::TOKEN_STRUCT::TOKEN_LT){
                    parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_LT);
                    ast->data_type += Types::typename_to_int(parser->token->value);
                    parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_ID);
                    parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_GT);
                }
            }
        }else{
            if (parser->token->type == Token::TOKEN_STRUCT::TOKEN_LPAREN){
                ast->type = AST::AST_STRUCT::AST_CALL; // 函数参数列表
                ast->value = parser_parse_list(parser);
            }
        }
        return ast;
    }

    AST::AST_T* Parser::parser_parse_int(Parser::parser_T* parser){
        int int_value = atoi(parser->token->value);
        parser_eat(parser, Token::TOKEN_STRUCT::TOKEN_INT);
        AST::AST_T* ast = AST::init_ast(AST::AST_STRUCT::AST_INT);
        ast->int_value = int_value;
        return ast;
    }

    AST::AST_T* Parser::parser_parse_expr(Parser::parser_T* parser){
        switch (parser->token->type) {
            case Token::TOKEN_STRUCT::TOKEN_ID:
                return Parser::parser_parse_id(parser);
            case Token::TOKEN_STRUCT::TOKEN_LPAREN:
                return Parser::parser_parse_list(parser);
            case Token::TOKEN_STRUCT::TOKEN_INT:
                return Parser::parser_parse_int(parser);
            default: {
                printf("[Parser]: Unexpected token '%s\n'", Token::token_to_str(parser->token));
                exit(1);
            }
        }
    }
}