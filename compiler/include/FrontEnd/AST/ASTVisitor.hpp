/* ASTVisitor.hpp - Common-use class to traverse the AST.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_FRONTEND_AST_AST_VISITOR_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_VISITOR_HPP

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

class ASTVisitor {
public:
  virtual ~ASTVisitor() noexcept = default;

  virtual void Visit(const ASTBinaryOperator *) const = 0;
  virtual void Visit(const ASTBooleanLiteral *) const = 0;
  virtual void Visit(const ASTBreakStmt *) const = 0;
  virtual void Visit(const ASTCompoundStmt *) const = 0;
  virtual void Visit(const ASTContinueStmt *) const = 0;
  virtual void Visit(const ASTDoWhileStmt *) const = 0;
  virtual void Visit(const ASTFloatingPointLiteral *) const = 0;
  virtual void Visit(const ASTForStmt *) const = 0;
  virtual void Visit(const ASTFunctionDecl *) const = 0;
  virtual void Visit(const ASTFunctionCall *) const = 0;
  virtual void Visit(const ASTIfStmt *) const = 0;
  virtual void Visit(const ASTIntegerLiteral *) const = 0;
  virtual void Visit(const ASTReturnStmt *) const = 0;
  virtual void Visit(const ASTStringLiteral *) const = 0;
  virtual void Visit(const ASTSymbol *) const = 0;
  virtual void Visit(const ASTUnaryOperator *) const = 0;
  virtual void Visit(const ASTVarDecl *) const = 0;
  virtual void Visit(const ASTWhileStmt *) const = 0;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_VISITOR_HPP
