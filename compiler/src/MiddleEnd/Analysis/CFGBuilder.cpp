/* CFGBuilder.cpp - Control Flow Graph from AST builder.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/Analysis/CFGBuilder.hpp"
#include "FrontEnd/AST/ASTBinaryOperator.hpp"
#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include "FrontEnd/AST/ASTDoWhileStmt.hpp"
#include "FrontEnd/AST/ASTFunctionDecl.hpp"
#include "FrontEnd/AST/ASTIfStmt.hpp"
#include "FrontEnd/AST/ASTPrettyPrint.hpp"
#include "FrontEnd/AST/ASTSymbol.hpp"
#include "FrontEnd/AST/ASTVarDecl.hpp"
#include "FrontEnd/AST/ASTWhileStmt.hpp"
#include "MiddleEnd/Analysis/SSAForm.hpp"
#include "MiddleEnd/IR/IRAssignment.hpp"
#include "MiddleEnd/IR/IRBranch.hpp"
#include "MiddleEnd/IR/IRPhiNode.hpp"

using namespace weak::frontEnd;

namespace weak {
namespace middleEnd {

CFGBuilder::CFGBuilder(
    const std::vector<std::unique_ptr<frontEnd::ASTNode>> &TheStatements)
    : Statements(TheStatements), CFGraph(), CurrentBlock(MakeBlock("Entry")),
      BlocksForVariable() {}

void CFGBuilder::Build() {
  for (const auto &Expression : Statements)
    Expression->Accept(this);
  BuildSSAForm();
}

CFGBlock *CFGBuilder::MakeBlock(std::string Label) const {
  int NextIndex = static_cast<int>(CFGraph.GetBlocks().size());
  auto *Block = new CFGBlock(NextIndex, Label);
  CFGraph.AddBlock(Block);
  return Block;
}

void CFGBuilder::MakeBranch(ASTNode *Condition, CFGBlock *ThenBlock,
                            CFGBlock *ElseBlock) const {
  CFGBlock::AddLink(CurrentBlock, ThenBlock);
  CFGBlock::AddLink(CurrentBlock, ElseBlock);
  CurrentBlock->AddStatement(new IRBranch(Condition, ThenBlock, ElseBlock));
}

void CFGBuilder::Visit(const frontEnd::ASTCompoundStmt *Stmt) const {
  for (const auto &Expression : Stmt->GetStmts())
    Expression->Accept(this);
}

void CFGBuilder::Visit(const frontEnd::ASTFunctionDecl *Stmt) const {
  Stmt->GetBody()->Accept(this);
}

void CFGBuilder::Visit(const frontEnd::ASTVarDecl *Stmt) const {
  BlocksForVariable[Stmt->GetSymbolName()].insert(CurrentBlock);
  CurrentBlock->AddStatement(new IRAssignment(
      new ASTSymbol(Stmt->GetSymbolName()), Stmt->GetDeclareBody().get()));
}

void CFGBuilder::Visit(const frontEnd::ASTBinaryOperator *Stmt) const {
  if (Stmt->GetOperation() == TokenType::ASSIGN) {
    const ASTSymbol *Symbol =
        static_cast<const ASTSymbol *>(Stmt->GetLHS().get());
    BlocksForVariable[Symbol->GetName()].insert(CurrentBlock);
    CurrentBlock->AddStatement(
        new IRAssignment(new ASTSymbol(*Symbol), Stmt->GetRHS().get()));
    return;
  }
  Stmt->GetLHS()->Accept(this);
  Stmt->GetRHS()->Accept(this);
}

void CFGBuilder::Visit(const frontEnd::ASTIfStmt *Stmt) const {
  CFGBlock *ConditionBlock = MakeBlock("Branch");
  CFGBlock *ThenBlock = MakeBlock("Then");
  CFGBlock *MergeBlock = MakeBlock("MergeBlock");
  CFGBlock *ElseBlock = nullptr;

  CFGBlock::AddLink(CurrentBlock, ConditionBlock);
  CurrentBlock = ConditionBlock;

  if (Stmt->GetElseBody()) {
    ElseBlock = MakeBlock("Else");
    MakeBranch(Stmt->GetCondition().get(), ThenBlock, ElseBlock);
  } else
    MakeBranch(Stmt->GetCondition().get(), ThenBlock, MergeBlock);

  CurrentBlock = ThenBlock;
  Stmt->GetThenBody()->Accept(this);

  if (Stmt->GetElseBody()) {
    CFGBlock::AddLink(ThenBlock, MergeBlock);
    CFGBlock::AddLink(ElseBlock, MergeBlock);
    CFGBlock::AddLink(ConditionBlock, ElseBlock);
    CurrentBlock = ElseBlock;
    Stmt->GetElseBody()->Accept(this);
  } else
    CFGBlock::AddLink(CurrentBlock, MergeBlock);

  CurrentBlock = MergeBlock;
}

void CFGBuilder::Visit(const frontEnd::ASTWhileStmt *Stmt) const {
  CFGBlock *ConditionBlock = MakeBlock("Branch");
  CFGBlock *BodyBlock = MakeBlock("Body");
  CFGBlock *MergeBlock = MakeBlock("MergeBlock");

  CFGBlock::AddLink(CurrentBlock, ConditionBlock);
  CFGBlock::AddLink(ConditionBlock, BodyBlock);
  CFGBlock::AddLink(ConditionBlock, MergeBlock);

  ConditionBlock->AddStatement(
      new IRBranch(Stmt->GetCondition().get(), BodyBlock, MergeBlock));

  CurrentBlock = BodyBlock;
  Stmt->GetBody()->Accept(this);
  CFGBlock::AddLink(CurrentBlock, ConditionBlock);
  CurrentBlock = MergeBlock;
}

void CFGBuilder::Visit(const frontEnd::ASTDoWhileStmt *Stmt) const {
  CFGBlock *ConditionBlock = MakeBlock("Branch");
  CFGBlock *BodyBlock = MakeBlock("Body");
  CFGBlock *MergeBlock = MakeBlock("MergeBlock");

  CFGBlock::AddLink(CurrentBlock, BodyBlock);
  CFGBlock::AddLink(ConditionBlock, BodyBlock);
  CFGBlock::AddLink(ConditionBlock, MergeBlock);

  ConditionBlock->AddStatement(
      new IRBranch(Stmt->GetCondition().get(), BodyBlock, MergeBlock));

  CurrentBlock = BodyBlock;
  Stmt->GetBody()->Accept(this);
  CFGBlock::AddLink(CurrentBlock, ConditionBlock);
  CurrentBlock = MergeBlock;
}

void CFGBuilder::InsertPhiNodes() {
  for (const auto &[VariableName, AssignedBlocks] : BlocksForVariable) {
    std::set<CFGBlock *> DominanceFrontier =
        CFGraph.GetDominanceFrontierForSubset(AssignedBlocks);
    for (auto *Block : DominanceFrontier) {
      std::map<CFGBlock *, ASTSymbol *> VariablesMap;
      for (auto *Predecessor : Block->Predecessors)
        VariablesMap[Predecessor] = new ASTSymbol(VariableName);
      auto *Phi = new IRPhiNode(std::make_unique<ASTSymbol>(VariableName),
                                std::move(VariablesMap));
      Block->Statements.insert(Block->Statements.begin(), Phi);
    }
  }
}

void CFGBuilder::BuildSSAForm() {
  CFGraph.CommitAllChanges();
  InsertPhiNodes();

  SSAForm SSABuilder(&CFGraph);
  for (const auto &[Variable, _] : BlocksForVariable)
    SSABuilder.Compute(Variable);
}

CFG &CFGBuilder::GetCFG() const { return CFGraph; }

} // namespace middleEnd
} // namespace weak