/* ASTBooleanLiteral.cpp - AST node to represent a boolean literal.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/AST/ASTBooleanLiteral.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTBooleanLiteral::ASTBooleanLiteral(bool TheValue, unsigned TheLineNo,
                                     unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Value(TheValue) {}

ASTType ASTBooleanLiteral::GetASTType() const {
  return ASTType::BOOLEAN_LITERAL;
}

void ASTBooleanLiteral::Accept(const ASTVisitor *Visitor) const {
  Visitor->Visit(this);
}

bool ASTBooleanLiteral::GetValue() const { return Value; }

} // namespace frontEnd
} // namespace weak