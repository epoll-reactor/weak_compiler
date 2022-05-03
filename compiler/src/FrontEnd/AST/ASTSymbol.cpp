/* ASTSymbol.cpp - AST node to represent a variable name.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/AST/ASTSymbol.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTSymbol::ASTSymbol(std::string TheValue, unsigned TheLineNo,
                     unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Value(std::move(TheValue)), SSAIndex(0) {
}

ASTType ASTSymbol::GetASTType() const { return ASTType::SYMBOL; }

void ASTSymbol::Accept(const ASTVisitor *Visitor) const {
  Visitor->Visit(this);
}

void ASTSymbol::SetSSAIndex(int Index) { SSAIndex = Index; }

const std::string &ASTSymbol::GetName() const { return Value; }

std::string ASTSymbol::GetSSAName() const {
  return Value + "#" + std::to_string(SSAIndex);
}

} // namespace frontEnd
} // namespace weak