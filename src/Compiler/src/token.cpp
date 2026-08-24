//
// Created by 15813 on 2026/7/17.
//

#include "../include/token.h"
#include "../../Interpreter_v1/include/Token.h"

#include <stdlib.h>

namespace Compiler::Token{
    token_T* init_token(const char* value, TOKEN_STRUCT::TokenType type){
        token_T* token = static_cast<token_T*>(calloc(1, sizeof(struct TOKEN_STRUCT)));
        token->value = value;
        token->type = type;
        return token;
    }
}
