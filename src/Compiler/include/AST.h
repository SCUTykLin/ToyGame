//
// Created by YangKai.Lin on 2026/7/17.
//

#ifndef TOYPROJECTS_COMPILER_AST_H
#define TOYPROJECTS_COMPILER_AST_H

#include "list.h"

namespace Compiler {
    namespace AST {
        typedef struct AST_STRUCT {
            enum ASTType{
                AST_COMPOUND = 0,
                AST_FUNCTION,
                AST_CALL,
                AST_ASSIGNMENT,
                AST_VARIABLE,
                AST_STATEMENT,
                AST_INT,
                AST_NOOP,
            };
            ASTType type;
            list::list_T* children;
            char* name;
            struct AST_STRUCT* value;
            int int_value;
            int data_type;
        }AST_T;
        AST_T* init_ast(AST_STRUCT::ASTType type);
    }
}

#endif //TOYPROJECTS_COMPILER_AST_H