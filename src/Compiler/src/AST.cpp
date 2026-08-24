//
// Created by YangKai.Lin on 2026/7/17.
//

#include "../include/AST.h"
#include <cstdlib>

namespace Compiler {
    AST::AST_T* AST::init_ast(AST::AST_STRUCT::ASTType type) {
        AST::AST_T* ast = static_cast<AST::AST_T*>(calloc(1, sizeof(struct AST_STRUCT)));
        ast->type = type;

        if (type == AST_STRUCT::AST_COMPOUND){
            ast->children = list::init_list(sizeof(struct AST_STRUCT*));
        }

        return ast;
    }
}