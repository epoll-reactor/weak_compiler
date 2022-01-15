/* ASTBreakStmt.cpp - AST node to represent a break statement.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/AST/ASTBreakStmt.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTBreakStmt::ASTBreakStmt(unsigned TheLineNo, unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo) {}

ASTType ASTBreakStmt::GetASTType() const { return ASTType::BREAK_STMT; }

void ASTBreakStmt::Accept(const ASTVisitor *Visitor) const {
  Visitor->Visit(this);
}

} // namespace frontEnd
} // namespace weak