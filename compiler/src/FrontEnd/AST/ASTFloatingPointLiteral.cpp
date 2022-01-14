/* ASTFloatingPointLiteral.cpp - AST node to represent a floating point number.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/AST/ASTFloatingPointLiteral.hpp"

namespace weak {
namespace frontEnd {

ASTFloatingPointLiteral::ASTFloatingPointLiteral(double TheValue,
                                                 unsigned TheLineNo,
                                                 unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Value(TheValue) {}

ASTType ASTFloatingPointLiteral::GetASTType() const {
  return ASTType::FLOATING_POINT_LITERAL;
}

double ASTFloatingPointLiteral::GetValue() const { return Value; }

} // namespace frontEnd
} // namespace weak