/* ASTFunctionCall.cpp - AST node to represent a function call statement.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/AST/ASTFunctionCall.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTFunctionCall::ASTFunctionCall(
    std::string &&TheName, std::vector<std::unique_ptr<ASTNode>> &&TheArguments,
    unsigned TheLineNo, unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Name(std::move(TheName)),
      Arguments(std::move(TheArguments)) {}

ASTType ASTFunctionCall::GetASTType() const { return ASTType::FUNCTION_CALL; }

void ASTFunctionCall::Accept(const ASTVisitor *Visitor) const {
  Visitor->Visit(this);
}

const std::string &ASTFunctionCall::GetName() const { return Name; }

const std::vector<std::unique_ptr<ASTNode>> &
ASTFunctionCall::GetArguments() const {
  return Arguments;
}

} // namespace frontEnd
} // namespace weak