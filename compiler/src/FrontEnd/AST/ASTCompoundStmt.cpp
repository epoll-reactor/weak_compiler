/* ASTCompoundStmt.cpp - AST node to represent a block of code.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTCompoundStmt::ASTCompoundStmt(std::vector<std::unique_ptr<ASTNode>> &&stmts,
                                 unsigned TheLineNo, unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Stmts(std::move(stmts)) {}

ASTType ASTCompoundStmt::GetASTType() const { return ASTType::COMPOUND_STMT; }

void ASTCompoundStmt::Accept(const ASTVisitor *Visitor) const {
  Visitor->Visit(this);
}

std::vector<std::unique_ptr<ASTNode>> &&ASTCompoundStmt::GetStmts() {
  return std::move(Stmts);
}

const std::vector<std::unique_ptr<ASTNode>> &ASTCompoundStmt::GetStmts() const {
  return Stmts;
}

} // namespace frontEnd
} // namespace weak