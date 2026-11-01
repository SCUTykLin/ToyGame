//
// Created by 15813 on 2026/8/22.
//
#include "Expr.h"

namespace CLOX_V1::Ast{
    std::any Assign::accept(Visitor* visitor) {
        return visitor->visitAssign(this);
    }

    std::any Binary::accept(Visitor* visitor){
        return visitor->visitBinary(this);
    }

    std::any Call::accept(Visitor* visitor) {
        return visitor->visitCall(this);
    }

    std::any Get::accept(Visitor* visitor) {
        return visitor->visitGet(this);
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

    std::any Logical::accept(Visitor* visitor) {
        return visitor->visitLogical(this);
    }

    std::any Set::accept(Visitor* visitor) {
        return visitor->visitSet(this);
    }

    std::any Super::accept(Visitor* visitor) {
        return visitor->visitSuper(this);
    }

    std::any This::accept(Visitor* visitor) {
        return visitor->visitThis(this);
    }

    std::any Variable::accept(Visitor* visitor) {
        return visitor->visitVariable(this);
    }

}