/* ASTFunctionDecl.cpp - AST node to represent a function itself.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/AST/ASTFunctionDecl.hpp"
#include "FrontEnd/Lex/Token.hpp"

namespace weak {
namespace frontEnd {

ASTFunctionDecl::ASTFunctionDecl(
    TokenType TheReturnType, std::string &&TheName,
    std::vector<std::unique_ptr<ASTNode>> &&TheArguments,
    std::unique_ptr<ASTCompoundStmt> &&TheBody, unsigned TheLineNo,
    unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), ReturnType(TheReturnType),
      Name(std::move(TheName)), Arguments(std::move(TheArguments)),
      Body(std::move(TheBody)) {}

ASTType ASTFunctionDecl::GetASTType() const { return ASTType::FUNCTION_DECL; }

TokenType ASTFunctionDecl::GetReturnType() const { return ReturnType; }

const std::string &ASTFunctionDecl::GetName() const { return Name; }

std::vector<std::unique_ptr<ASTNode>> &&ASTFunctionDecl::GetArguments() {
  return std::move(Arguments);
}

const std::vector<std::unique_ptr<ASTNode>> &
ASTFunctionDecl::GetArguments() const {
  return Arguments;
}

std::unique_ptr<ASTCompoundStmt> &&ASTFunctionDecl::GetBody() {
  return std::move(Body);
}

const std::unique_ptr<ASTCompoundStmt> &ASTFunctionDecl::GetBody() const {
  return Body;
}

} // namespace frontEnd
} // namespace weak