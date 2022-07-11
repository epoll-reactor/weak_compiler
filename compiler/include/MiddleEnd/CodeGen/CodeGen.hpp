/* CodeGen.hpp - LLVM IR generator.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_CODEGEN_HPP
#define WEAK_COMPILER_MIDDLE_END_CODEGEN_HPP

#include "FrontEnd/AST/ASTVisitor.hpp"

#include "llvm/IR/Value.h"

namespace weak {
namespace frontEnd {

class ASTNode;
class ASTBinaryOperator;
class ASTBooleanLiteral;
class ASTBreakStmt;
class ASTCompoundStmt;
class ASTContinueStmt;
class ASTDoWhileStmt;
class ASTFloatingPointLiteral;
class ASTForStmt;
class ASTFunctionDecl;
class ASTFunctionCall;
class ASTIfStmt;
class ASTIntegerLiteral;
class ASTReturnStmt;
class ASTStringLiteral;
class ASTSymbol;
class ASTUnaryOperator;
class ASTVarDecl;
class ASTWhileStmt;

} // namespace frontEnd
} // namespace weak

namespace weak {
namespace middleEnd {

class CodeGen : private frontEnd::ASTVisitor {
public:
  CodeGen(frontEnd::ASTNode *TheRoot);

  void CreateCode();

private:
  virtual void
  Visit(const frontEnd::ASTBinaryOperator *) const override; // Implemented.
  virtual void Visit(const frontEnd::ASTBooleanLiteral *) const override {}
  virtual void Visit(const frontEnd::ASTBreakStmt *) const override {}
  virtual void
  Visit(const frontEnd::ASTCompoundStmt *) const override; // Implemented.
  virtual void Visit(const frontEnd::ASTContinueStmt *) const override {}
  virtual void Visit(const frontEnd::ASTDoWhileStmt *) const override {}
  virtual void Visit(const frontEnd::ASTFloatingPointLiteral *) const override {
  }
  virtual void Visit(const frontEnd::ASTForStmt *) const override {}
  virtual void
  Visit(const frontEnd::ASTFunctionDecl *) const override; // Implemented.
  virtual void
  Visit(const frontEnd::ASTFunctionCall *) const override; // Implemented.
  virtual void Visit(const frontEnd::ASTIfStmt *) const override {}
  virtual void
  Visit(const frontEnd::ASTIntegerLiteral *) const override; // Implemented.
  virtual void
  Visit(const frontEnd::ASTReturnStmt *) const override; // Implemented.
  virtual void Visit(const frontEnd::ASTStringLiteral *) const override {}
  virtual void
  Visit(const frontEnd::ASTSymbol *) const override; // Implemented.
  virtual void Visit(const frontEnd::ASTUnaryOperator *) const override {}
  virtual void
  Visit(const frontEnd::ASTVarDecl *) const override; // Implemented.
  virtual void Visit(const frontEnd::ASTWhileStmt *) const override {}

  frontEnd::ASTNode *Root;
  mutable llvm::Value *LastEmitted;
  mutable bool IsReturnValue;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_CODEGEN_HPP
