//
// Created by 15813 on 2026/8/22.
//
#include "Expr.h"

namespace CLOX_V1::Ast{
    std::any Binary::accept(Visitor* visitor){
        return visitor->visitBinary(this);
    }

    std::any Grouping::accept(Visitor* visitor){
        return visitor->visitGrouping(this);
    }

    std::any Unary::accept(Visitor* visitor) {
        return visitor->visitUnary(this);
    }

    std::any Literal::accept(Visitor* visitor) {
        return visitor->visitLiteral(this);
    }

}