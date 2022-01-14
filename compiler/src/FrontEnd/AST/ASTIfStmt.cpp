/* ASTIfStmt.cpp - AST node to represent a if or if-else statements.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/AST/ASTIfStmt.hpp"

namespace weak {
namespace frontEnd {

ASTIfStmt::ASTIfStmt(std::unique_ptr<ASTNode> &&TheCondition,
                     std::unique_ptr<ASTCompoundStmt> &&TheThenBody,
                     std::unique_ptr<ASTCompoundStmt> &&TheElseBody,
                     unsigned TheLineNo, unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Condition(std::move(TheCondition)),
      ThenBody(std::move(TheThenBody)), ElseBody(std::move(TheElseBody)) {}

ASTType ASTIfStmt::GetASTType() const { return ASTType::IF_STMT; }

std::unique_ptr<ASTNode> &&ASTIfStmt::GetCondition() {
  return std::move(Condition);
}

const std::unique_ptr<ASTNode> &ASTIfStmt::GetCondition() const {
  return Condition;
}

std::unique_ptr<ASTCompoundStmt> &&ASTIfStmt::GetThenBody() {
  return std::move(ThenBody);
}

const std::unique_ptr<ASTCompoundStmt> &ASTIfStmt::GetThenBody() const {
  return ThenBody;
}

std::unique_ptr<ASTCompoundStmt> &&ASTIfStmt::GetElseBody() {
  return std::move(ElseBody);
}

const std::unique_ptr<ASTCompoundStmt> &ASTIfStmt::GetElseBody() const {
  return ElseBody;
}

} // namespace frontEnd
} // namespace weak