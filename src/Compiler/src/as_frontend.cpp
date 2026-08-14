//
// Created by 15813 on 2026/8/14.
//
#include "../include/as_frontend.h"
#include <cstring>
#include <cstdio>

namespace Compiler::code{

    char* as_f_compound(AST::AST_T* ast){
        char* value = static_cast<char*>(calloc(1, sizeof(char)));
        for (int i = 0; i < (int)ast->children->size; ++i){
            AST::AST_T* child_ast = (AST::AST_T*)ast->children->items[i];
            char* next_value = as_f(child_ast);
            value = static_cast<char *>(realloc(value, (strlen(next_value) + 1) * sizeof(char)));
            strcat(value, next_value);
        }
        return value;
    }

    char* as_f_assignment(AST::AST_T* ast){
        const char* example = "mov $128, \%eax";
        char* s = static_cast<char*>(calloc(strlen(example) + 1, sizeof(char)));
        strcpy(s, example);
        return s;
    }
    char* as_f_variable(AST::AST_T* ast){return nullptr;}
    char* as_f_call(AST::AST_T* ast){return nullptr;}
    char* as_f_int(AST::AST_T* ast){return nullptr;}

    char* as_f(AST::AST_T* ast){
        char* value = static_cast<char*>(calloc(1, sizeof(char)));
        char* next_value = nullptr;
        switch (ast->type) {
            case AST::AST_STRUCT::AST_COMPOUND:
                next_value = as_f_compound(ast);
                break;
            case AST::AST_STRUCT::AST_ASSIGNMENT:
                next_value = as_f_assignment(ast);
                break;
            case AST::AST_STRUCT::AST_VARIABLE:
                next_value = as_f_variable(ast);
                break;
            case AST::AST_STRUCT::AST_CALL:
                next_value = as_f_call(ast);
                break;
            case AST::AST_STRUCT::AST_INT:
                next_value = as_f_int(ast);
                break;
            default:
                printf("[As Frontend]: No frontend for AST of type '%d'\n", ast->type);
                exit(1);
        }
        if (next_value) {
            value = static_cast<char *>(realloc(value, (strlen(next_value) + 1) * sizeof(char)));
            strcat(value, next_value);
        }
        return value;
    }
}