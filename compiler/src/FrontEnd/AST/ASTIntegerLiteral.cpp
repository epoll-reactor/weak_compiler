/* ASTIntegerLiteral.cpp - AST node to represent a integer number.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/AST/ASTIntegerLiteral.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTIntegerLiteral::ASTIntegerLiteral(signed TheValue, unsigned TheLineNo,
                                     unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Value(TheValue) {}

ASTType ASTIntegerLiteral::GetASTType() const {
  return ASTType::INTEGER_LITERAL;
}

void ASTIntegerLiteral::Accept(const ASTVisitor *Visitor) const {
  Visitor->Visit(this);
}

signed ASTIntegerLiteral::GetValue() const { return Value; }

} // namespace frontEnd
} // namespace weak