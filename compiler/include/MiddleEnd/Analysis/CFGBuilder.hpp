/* CFGBuilder.hpp - Control Flow Graph from AST builder.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_ANALYSIS_CFG_BUILDER_HPP
#define WEAK_COMPILER_MIDDLE_END_ANALYSIS_CFG_BUILDER_HPP

#include "FrontEnd/AST/ASTVisitor.hpp"
#include "MiddleEnd/Analysis/CFG.hpp"
#include "MiddleEnd/Analysis/CFGBlock.hpp"
#include <memory>

namespace weak {
namespace middleEnd {

/// \brief The builder of Control Flow Graph.
///
/// Implemented as visitor since operates on AST.
class CFGBuilder : private frontEnd::ASTVisitor {
public:
  CFGBuilder(const std::vector<std::unique_ptr<frontEnd::ASTNode>> &);

  void Build();

  CFG &GetCFG() const;

private:
  void Visit(const frontEnd::ASTBooleanLiteral *) const override {}
  void Visit(const frontEnd::ASTBreakStmt *) const override {}
  void Visit(const frontEnd::ASTContinueStmt *) const override {}
  void Visit(const frontEnd::ASTFloatingPointLiteral *) const override {}
  void Visit(const frontEnd::ASTFunctionCall *) const override {}
  void Visit(const frontEnd::ASTSymbol *) const override {}
  void Visit(const frontEnd::ASTIntegerLiteral *) const override {}
  void Visit(const frontEnd::ASTReturnStmt *) const override {}
  void Visit(const frontEnd::ASTStringLiteral *) const override {}
  void Visit(const frontEnd::ASTUnaryOperator *) const override {}

  /// Insert analyzed variable declaration to \ref BlocksForVariable.
  void Visit(const frontEnd::ASTVarDecl *) const override;

  /// Insert analyzed variable declaration to \ref BlocksForVariable
  /// if we have an assignment.
  void Visit(const frontEnd::ASTBinaryOperator *) const override;
  void Visit(const frontEnd::ASTCompoundStmt *) const override;
  void Visit(const frontEnd::ASTFunctionDecl *) const override;
  void Visit(const frontEnd::ASTIfStmt *) const override;
  void Visit(const frontEnd::ASTWhileStmt *) const override;
  void Visit(const frontEnd::ASTDoWhileStmt *) const override;
  void Visit(const frontEnd::ASTForStmt *) const override;

  /// Allocate the new block with unique label.
  CFGBlock *MakeBlock(std::string Label) const;

  /// Helper function to insert branches to \ref CurrentBlock.
  void MakeBranch(frontEnd::ASTNode *Condition, CFGBlock *ThenBlock,
                  CFGBlock *ElseBlock) const;

  void InsertPhiNodes();
  void BuildSSAForm();

  /// Simple reference to our AST stuff.
  const std::vector<std::unique_ptr<frontEnd::ASTNode>> &StatementsRef;

  /// Generated Control Flow Graph.
  mutable CFG CFGraph;

  /// Helper pointer to simplify code design.
  mutable CFGBlock *CurrentBlock;

  /// Mapping variables to blocks where they are assigned. Used
  /// to decide where to put Phi-nodes.
  mutable std::map<std::string, std::set<CFGBlock *>> BlocksForVariable;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_ANALYSIS_CFG_BUILDER_HPP