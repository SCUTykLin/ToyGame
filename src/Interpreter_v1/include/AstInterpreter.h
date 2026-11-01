//
// Created by YangKai.Lin on 2026/11/1.
//

#ifndef TOYPROJECTS_Interpreter_v1_INTERPRETER_H
#define TOYPROJECTS_Interpreter_v1_INTERPRETER_H

#include "Expr.h"

namespace CLOX_V1::Ast {
    class AstInterpreter : public Visitor {
    public:
        std::any visitLiteral(Literal *literal) override;
    };
}

#endif //TOYPROJECTS_Interpreter_v1_INTERPRETER_H
