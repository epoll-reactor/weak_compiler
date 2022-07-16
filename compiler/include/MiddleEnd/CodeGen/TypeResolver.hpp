/* TypeResolver.hpp - Helper class to translate frontend types to LLVM.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_TYPE_RESOLVER_HPP
#define WEAK_COMPILER_MIDDLE_END_TYPE_RESOLVER_HPP

#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/AST/ASTVarDecl.hpp"
#include "FrontEnd/Lex/Token.hpp"

namespace llvm {
class LLVMContext;
class Type;
} // namespace llvm

namespace weak {
namespace middleEnd {

/// Helper class to translate frontend types to LLVM.
class TypeResolver {
public:
  TypeResolver(llvm::LLVMContext &);

  /// Convert given parameter (including void) to corresponding LLVM type.
  llvm::Type *Resolve(frontEnd::TokenType);
  /// \copydoc TypeResolver::Resolve(frontEnd::TokenType)
  llvm::Type *Resolve(const frontEnd::ASTNode *);

  /// Convert given parameter (excluding void) to corresponding LLVM type.
  llvm::Type *ResolveExceptVoid(frontEnd::TokenType);
  /// \copydoc TypeResolver::ResolveExceptVoid(frontEnd::TokenType)
  llvm::Type *ResolveExceptVoid(const frontEnd::ASTNode *);

private:
  /// Convert frontEnd::ASTNode to frontEnd::ASTVarDecl, or
  /// emit compile error on conversion error.
  const frontEnd::ASTVarDecl *GetVarDecl(const frontEnd::ASTNode *);

  /// The reference to main LLVM context.
  llvm::LLVMContext &LLVMCtx;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_TYPE_RESOLVER_HPP
