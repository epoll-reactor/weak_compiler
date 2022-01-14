/* ASTBooleanLiteral.cpp - AST node to represent a boolean literal.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/AST/ASTBooleanLiteral.hpp"

namespace weak {
namespace frontEnd {

ASTBooleanLiteral::ASTBooleanLiteral(bool TheValue, unsigned TheLineNo,
                                     unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Value(TheValue) {}

ASTType ASTBooleanLiteral::GetASTType() const {
  return ASTType::BOOLEAN_LITERAL;
}

bool ASTBooleanLiteral::GetValue() const { return Value; }

} // namespace frontEnd
} // namespace weak