//
// Created by 15813 on 2026/8/14.
//
#include "../include/as_frontend.h"
#include <cstring>
#include <cstdio>
#include <algorithm>

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
        char* s = nullptr;
        if (ast->value->type == AST::AST_T::AST_FUNCTION) {
            const char *template_ = ".globl %s\n"
                                    "%s:\n";
            s = static_cast<char *>(calloc(strlen(template_) + strlen(ast->name) * 2 + 1, sizeof(char)));
            sprintf(s, template_, ast->name, ast->name);

            AST::AST_T *as_val = ast->value;
            char *as_val_val = as_f(as_val->value);

            s = static_cast<char *>(realloc(s, (strlen(s) + strlen(as_val_val) + 1) * sizeof(char)));
            strcat(s, as_val_val);
        }
        return s;
    }
    char* as_f_variable(AST::AST_T* ast){return nullptr;}
    char* as_f_call(AST::AST_T* ast){
        char* s = static_cast<char*>(calloc(1, sizeof(char)));
        if (strcmp(ast->name, "return") == 0){
            AST::AST_T* first_arg = static_cast<AST::AST_T*>(ast->value->children->size ? ast->value->children->items[0] : nullptr);
            const char* template_ = "mov $%d, %%eax\n"
                                    "ret\n";
            char* ret_s = static_cast<char*>(calloc(strlen(template_) + 128, sizeof(char)));
            sprintf(ret_s, template_, first_arg ? first_arg->int_value : 0);
            s = static_cast<char*>(realloc(s, (strlen(ret_s) + 1) * sizeof(char)));
            strcat(s, ret_s);
        }
        return s;
    }
    char* as_f_int(AST::AST_T* ast){return nullptr;}

    char* as_f_root(AST::AST_T* ast){
        const char* section_text = ".section .text\n"
                                   ".globl _start\n"
                                   "_start:\n"
                                   "call main\n"
                                   "mov %%eax, %%ebx\n"
                                   "mov $1, %%eax\n"
                                   "int $0x80\n\n";
        char* value = static_cast<char*>(calloc((strlen(section_text) + 128), sizeof(char)));
        strcpy(value, section_text);
        char* next_value = as_f(ast);

        if (next_value) {
            value = static_cast<char *>(realloc(value, (strlen(value) + strlen(next_value) + 1) * sizeof(char)));
            strcat(value, next_value);
        }
        return value;
    }

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