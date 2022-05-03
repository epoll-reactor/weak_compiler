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

  void Visit(const frontEnd::ASTVarDecl *) const override;
  void Visit(const frontEnd::ASTCompoundStmt *) const override;
  void Visit(const frontEnd::ASTFunctionDecl *) const override;
  void Visit(const frontEnd::ASTIfStmt *) const override;
  void Visit(const frontEnd::ASTBinaryOperator *) const override;
  void Visit(const frontEnd::ASTWhileStmt *) const override;
  void Visit(const frontEnd::ASTDoWhileStmt *) const override;
  void Visit(const frontEnd::ASTForStmt *) const override;

  CFGBlock *MakeBlock(std::string Label) const;
  void MakeBranch(frontEnd::ASTNode *Condition, CFGBlock *ThenBlock,
                  CFGBlock *ElseBlock) const;

  void InsertPhiNodes();
  void BuildSSAForm();
  /// Carefully remove all empty nodes from CFG.
  /// \todo Reindex CFG blocks numbers.
  void ReduceGraph();

  const std::vector<std::unique_ptr<frontEnd::ASTNode>> &Statements;

  mutable CFG CFGraph;

  mutable CFGBlock *CurrentBlock;

  mutable std::map<std::string, std::set<CFGBlock *>> BlocksForVariable;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_ANALYSIS_CFG_BUILDER_HPP