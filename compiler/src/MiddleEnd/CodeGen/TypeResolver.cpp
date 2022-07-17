/* TypeResolver.cpp - Helper class to translate frontend types to LLVM.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/CodeGen/TypeResolver.hpp"
#include "FrontEnd/AST/ASTVarDecl.hpp"
#include "Utility/Diagnostic.hpp"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"

namespace weak {
namespace middleEnd {

TypeResolver::TypeResolver(llvm::LLVMContext &TheLLVMCtx)
    : LLVMCtx(TheLLVMCtx) {}

const frontEnd::ASTVarDecl *
TypeResolver::GetVarDecl(const frontEnd::ASTNode *Node) {
  using frontEnd::ASTType;
  if (Node->GetASTType() != ASTType::VAR_DECL) {
    unsigned LineNo = Node->GetLineNo();
    unsigned ColumnNo = Node->GetColumnNo();
    weak::CompileError(LineNo, ColumnNo) << "Expected parameter";
  }

  const auto *Decl = static_cast<const frontEnd::ASTVarDecl *>(Node);
  return Decl;
}

llvm::Type *TypeResolver::Resolve(frontEnd::TokenType T) {
  using frontEnd::TokenType;
  switch (T) {
  case TokenType::VOID:
    return llvm::Type::getVoidTy(LLVMCtx);
  case TokenType::INT:
    return llvm::Type::getInt32Ty(LLVMCtx);
  case TokenType::BOOLEAN:
    return llvm::Type::getInt32Ty(LLVMCtx);
  case TokenType::FLOAT:
    return llvm::Type::getFloatTy(LLVMCtx);
  default:
    weak::CompileError() << "Wrong type: " << frontEnd::TokenToString(T);
    weak::UnreachablePoint("");
  }
}

llvm::Type *TypeResolver::Resolve(const frontEnd::ASTNode *Node) {
  const frontEnd::ASTVarDecl *Decl = GetVarDecl(Node);
  return Resolve(Decl->GetDataType());
}

llvm::Type *TypeResolver::ResolveExceptVoid(frontEnd::TokenType T) {
  using frontEnd::TokenType;
  switch (T) {
  case TokenType::INT:
    return llvm::Type::getInt32Ty(LLVMCtx);
  case TokenType::FLOAT:
    return llvm::Type::getFloatTy(LLVMCtx);
  case TokenType::BOOLEAN:
    return llvm::Type::getInt32Ty(LLVMCtx);
  default:
    weak::CompileError() << "Wrong type: " << frontEnd::TokenToString(T);
    weak::UnreachablePoint("");
  }
}

llvm::Type *TypeResolver::ResolveExceptVoid(const frontEnd::ASTNode *Node) {
  const frontEnd::ASTVarDecl *Decl = GetVarDecl(Node);
  return ResolveExceptVoid(Decl->GetDataType());
}

} // namespace middleEnd
} // namespace weak