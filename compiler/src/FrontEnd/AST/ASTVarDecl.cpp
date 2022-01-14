/* ASTVarDecl.cpp - AST node to represent a variable declaration.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/AST/ASTVarDecl.hpp"

namespace weak {
namespace frontEnd {

ASTVarDecl::ASTVarDecl(TokenType TheDataType, std::string &&TheSymbolName,
                       std::unique_ptr<ASTNode> &&TheDeclareBody,
                       unsigned TheLineNo, unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), DataType(TheDataType),
      SymbolName(std::move(TheSymbolName)),
      DeclareBody(std::move(TheDeclareBody)) {}

ASTType ASTVarDecl::GetASTType() const { return ASTType::VAR_DECL; }

TokenType ASTVarDecl::GetDataType() const { return DataType; }

const std::string &ASTVarDecl::GetSymbolName() const { return SymbolName; }

const std::unique_ptr<ASTNode> &ASTVarDecl::GetDeclareBody() const {
  return DeclareBody;
}

} // namespace frontEnd
} // namespace weak