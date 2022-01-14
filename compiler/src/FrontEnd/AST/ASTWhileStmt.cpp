/* ASTWhileStmt.cpp - AST node to represent a while statement.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/AST/ASTWhileStmt.hpp"

namespace weak {
namespace frontEnd {

ASTWhileStmt::ASTWhileStmt(std::unique_ptr<ASTNode> &&TheCondition,
                           std::unique_ptr<ASTCompoundStmt> &&TheBody,
                           unsigned TheLineNo, unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Condition(std::move(TheCondition)),
      Body(std::move(TheBody)) {}

ASTType ASTWhileStmt::GetASTType() const { return ASTType::WHILE_STMT; }

std::unique_ptr<ASTNode> &&ASTWhileStmt::GetCondition() {
  return std::move(Condition);
}

const std::unique_ptr<ASTNode> &ASTWhileStmt::GetCondition() const {
  return Condition;
}

std::unique_ptr<ASTCompoundStmt> &&ASTWhileStmt::GetBody() {
  return std::move(Body);
}

const std::unique_ptr<ASTCompoundStmt> &ASTWhileStmt::GetBody() const {
  return Body;
}

} // namespace frontEnd
} // namespace weak