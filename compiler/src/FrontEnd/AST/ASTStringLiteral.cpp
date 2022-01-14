/* ASTStringLiteral.cpp - AST node to represent a string literal.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/AST/ASTStringLiteral.hpp"

namespace weak {
namespace frontEnd {

ASTStringLiteral::ASTStringLiteral(std::string TheValue, unsigned TheLineNo,
                                   unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Value(std::move(TheValue)) {}

ASTType ASTStringLiteral::GetASTType() const { return ASTType::STRING_LITERAL; }

const std::string &ASTStringLiteral::GetValue() const { return Value; }

} // namespace frontEnd
} // namespace weak