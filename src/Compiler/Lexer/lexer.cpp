//
// Created by YangKai.Lin on 2026/7/11.
//

#include "lexer.h"
#include <iostream>

namespace compiler {
    Lexer::Lexer(FILE *file) {
        char current = fgetc(file);
        while (current != EOF) {
            if (current == ';') {
                std::cout << "FPUND SEMICOLON\n" << std::endl;
            }
            else if (current == '(') {
                std::cout << "FPUND OPEN PARE\n" << std::endl;
            }else if (current == ')') {
                std::cout << "FPUND CLOSE PARE\n" << std::endl;
            }else if (isdigit(current)) {
                TokenLiteral* test_token = generate_number(current, file);
                printf("TEST TOKEN VALUE: %s\n", test_token->value);
            }else if (isalpha(current)) {
                TokenKeyword* test_keyword = generate_keyword(current, file);
            }
            current = fgetc(file);
        }
    }

    TokenLiteral* Lexer::generate_number(char current, FILE *file) {
        TokenLiteral* token = static_cast<TokenLiteral*>(malloc(sizeof(TokenLiteral)));
        token->type = INT;
        char* value = static_cast<char*>(malloc(sizeof(char) * 4));
        int value_index = 0;
        while (isdigit(current) && current != EOF) {
            if (!isdigit(current)) {
                break;
            }
            value[value_index++] = current;
            printf("%c", current);
            current = fgetc(file);
        }
        value[value_index] = '\0';
        token->value = value;
        printf("test token: %s\n", token->value);
        return token;
    }

    TokenKeyword* Lexer::generate_keyword(char current, FILE* file) {
        TokenKeyword* token = static_cast<TokenKeyword*>(malloc(sizeof(TokenKeyword)));
        char* keyword = static_cast<char*>(malloc(sizeof(char) * 4));
        int keyword_index = 0;
        while (isalpha(current) && current != EOF) {
            keyword[keyword_index++] = current;
            current = fgetc(file);
        }
        keyword[keyword_index] = '\0';
        if (!strcmp(keyword, "exit")) {
            std::cout << keyword << std::endl;
            token->type = EXIT;
            printf("TOKEN TYPE EXIT\n");
        }
        return token;
    }


}
