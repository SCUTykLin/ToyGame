//
// Created by YangKai.Lin on 2026/11/1.
//
#include "Stmt.h"

namespace CLOX_V1::Ast {
    // 每个 accept 只做一件事：把自己转交给 visitor 上对应的方法。
    // 类型在这里是静态确定的，所以 visitor 那边可以安心收 Block* / Print* 这类具体指针。
    std::any Block::accept(StmtVisitor* visitor) {
        return visitor->visitBlockStmt(this);
    }

    std::any Expression::accept(StmtVisitor* visitor) {
        return visitor->visitExpressionStmt(this);
    }

    std::any Print::accept(StmtVisitor* visitor) {
        return visitor->visitPrintStmt(this);
    }

    std::any Var::accept(StmtVisitor* visitor) {
        return visitor->visitVarStmt(this);
    }
}
