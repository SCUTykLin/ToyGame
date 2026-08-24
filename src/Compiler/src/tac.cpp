//
// Created by 15813 on 2026/7/17.
//

#include "../include/tac.h"
#include "../include/lexer.h"
#include "../include/io.h"
#include "../include/parser.h"
#include "../include/as_frontend.h"
#include <cstdlib>
#include <cstring>

namespace Compiler::tac {

    static char* sh(char* cmd){
        char* output = (char*) calloc(1, sizeof(char));
        output[0] = '\0';

        FILE* fp;
        char path[1035];

        fp = _popen(cmd, "r");

        if (fp == nullptr){
            printf("failed to run command\n");
            exit(1);
        }

        while (fgets(path, sizeof(path), fp) != nullptr){
            output = (char*) realloc(output, (strlen(output) + strlen(path) + 1) * sizeof(char));
            strcat(output, path);
        }
        _pclose(fp);
        free(cmd);
        return output;
    }

    void tac_compile(char* src) {
        Lexer::lexer_T* lexer = Lexer::init_lexer(src);
        Parser::parser_T* parser = Parser::init_parser(lexer);
        AST::AST_T* root = Parser::parser_parse(parser);
        char* s = code::as_f_root(root);
        printf("%s\n", s);
        IO::tac_write_file("a.s", s);

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