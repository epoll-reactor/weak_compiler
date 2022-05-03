/* VariableSearchVisitor.hpp - Recursive variable searcher in AST and IR.
 * statements. Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_IR_VARIABLE_SEARCH_VISITOR_HPP
#define WEAK_COMPILER_MIDDLE_END_IR_VARIABLE_SEARCH_VISITOR_HPP

#include "FrontEnd/AST/ASTVisitor.hpp"
#include "MiddleEnd/IR/IRNode.hpp"
#include "MiddleEnd/IR/IRVisitor.hpp"
#include <set>

namespace weak {
namespace middleEnd {

/// This simply walks through IR statements and return founded variables.
class VariableSearchVisitor : private frontEnd::ASTVisitor, private IRVisitor {
public:
  std::set<frontEnd::ASTSymbol *> AllVarsUsedInStatement(IRNode *);

private:
  void Visit(const frontEnd::ASTBooleanLiteral *) const override {}
  void Visit(const frontEnd::ASTBreakStmt *) const override {}
  void Visit(const frontEnd::ASTCompoundStmt *) const override {}
  void Visit(const frontEnd::ASTContinueStmt *) const override {}
  void Visit(const frontEnd::ASTDoWhileStmt *) const override {}
  void Visit(const frontEnd::ASTFloatingPointLiteral *) const override {}
  void Visit(const frontEnd::ASTForStmt *) const override {}
  void Visit(const frontEnd::ASTFunctionDecl *) const override {}
  void Visit(const frontEnd::ASTFunctionCall *) const override {}
  void Visit(const frontEnd::ASTIfStmt *) const override {}
  void Visit(const frontEnd::ASTIntegerLiteral *) const override {}
  void Visit(const frontEnd::ASTReturnStmt *) const override {}
  void Visit(const frontEnd::ASTStringLiteral *) const override {}
  void Visit(const frontEnd::ASTUnaryOperator *) const override {}
  void Visit(const frontEnd::ASTVarDecl *) const override {}
  void Visit(const frontEnd::ASTWhileStmt *) const override {}

  void Visit(const frontEnd::ASTSymbol *) const override;
  void Visit(const frontEnd::ASTBinaryOperator *) const override;
  void Visit(const IRAssignment *) const override;
  void Visit(const IRBranch *) const override;

  mutable std::set<frontEnd::ASTSymbol *> Variables;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_IR_VARIABLE_SEARCH_VISITOR_HPP
