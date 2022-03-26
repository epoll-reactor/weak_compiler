/* IRNodePrinter.hpp - Simplified AST dump for IR statements.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_IR_IR_NODE_PRINTER_HPP
#define WEAK_COMPILER_MIDDLE_END_IR_IR_NODE_PRINTER_HPP

#include "FrontEnd/AST/ASTVisitor.hpp"
#include <iosfwd>

namespace weak {
namespace middleEnd {

class IRNodePrinter : private frontEnd::ASTVisitor {
public:
  IRNodePrinter(frontEnd::ASTNode *TheRootNode, std::ostream &TheOutStream);

  void Print();

private:
  void Visit(const frontEnd::ASTBinaryOperator *) const override;
  void Visit(const frontEnd::ASTBooleanLiteral *) const override;
  void Visit(const frontEnd::ASTBreakStmt *) const override;
  void Visit(const frontEnd::ASTCompoundStmt *) const override;
  void Visit(const frontEnd::ASTContinueStmt *) const override;
  void Visit(const frontEnd::ASTDoWhileStmt *) const override;
  void Visit(const frontEnd::ASTFloatingPointLiteral *) const override;
  void Visit(const frontEnd::ASTForStmt *) const override;
  void Visit(const frontEnd::ASTFunctionDecl *) const override;
  void Visit(const frontEnd::ASTFunctionCall *) const override;
  void Visit(const frontEnd::ASTIfStmt *) const override;
  void Visit(const frontEnd::ASTIntegerLiteral *) const override;
  void Visit(const frontEnd::ASTReturnStmt *) const override;
  void Visit(const frontEnd::ASTStringLiteral *) const override;
  void Visit(const frontEnd::ASTSymbol *) const override;
  void Visit(const frontEnd::ASTUnaryOperator *) const override;
  void Visit(const frontEnd::ASTVarDecl *) const override;
  void Visit(const frontEnd::ASTWhileStmt *) const override;

  frontEnd::ASTNode *RootNode;
  std::ostream &OutStream;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_IR_IR_NODE_PRINTER_HPP