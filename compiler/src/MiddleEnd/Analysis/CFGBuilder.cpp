/* CFGBuilder.cpp - Control Flow Graph from AST builder.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/Analysis/CFGBuilder.hpp"
#include "FrontEnd/AST/ASTBinaryOperator.hpp"
#include "FrontEnd/AST/ASTCompoundStmt.hpp"
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

CFGBuilder::CFGBuilder()
    : CFGraph(new CFG), CurrentBlock(CreateBlock("Entry")) {}

CFGBlock *CFGBuilder::CreateBlock(std::string Label) const {
  int NextIndex = static_cast<int>(CFGraph->GetBlocks().size());
  auto *Block = new CFGBlock(NextIndex, Label);
  CFGraph->AddBlock(Block);
  return Block;
}

void CFGBuilder::CreateBranch(CFGBlock *Target) const {
  CFGBlock::AddLink(CurrentBlock, Target);
  CurrentBlock->AddStatement(std::make_unique<IRBranch>(Target));
}

void CFGBuilder::CreateConditionalBranch(ASTNode *Condition,
                                         CFGBlock *ThenBlock,
                                         CFGBlock *ElseBlock) const {
  CFGBlock::AddLink(CurrentBlock, ThenBlock);
  CFGBlock::AddLink(CurrentBlock, ElseBlock);
  CurrentBlock->AddStatement(
      std::make_unique<IRBranch>(Condition, ThenBlock, ElseBlock));
}

void CFGBuilder::Traverse(ASTNode *Expr) const { Expr->Accept(this); }

void CFGBuilder::Visit(const frontEnd::ASTCompoundStmt *Stmt) const {
  for (const auto &Expression : Stmt->GetStmts())
    Expression->Accept(this);
}

void CFGBuilder::Visit(const frontEnd::ASTFunctionDecl *Stmt) const {
  Stmt->GetBody()->Accept(this);
}

void CFGBuilder::Visit(const frontEnd::ASTVarDecl *Stmt) const {
  BlocksForVariable[Stmt->GetSymbolName()].insert(CurrentBlock);
  CurrentBlock->AddStatement(std::make_unique<IRAssignment>(
      new ASTSymbol(Stmt->GetSymbolName()), Stmt->GetDeclareBody().get()));
}

void CFGBuilder::Visit(const frontEnd::ASTBinaryOperator *Stmt) const {
  if (Stmt->GetOperation() == TokenType::ASSIGN) {
    BlocksForVariable[static_cast<const ASTSymbol *>(Stmt->GetLHS().get())
                          ->GetName()]
        .insert(CurrentBlock);
    CurrentBlock->AddStatement(std::make_unique<IRAssignment>(
        Stmt->GetLHS().get(), Stmt->GetRHS().get()));
    return;
  }
  Traverse(Stmt->GetLHS().get());
  Traverse(Stmt->GetRHS().get());
}

void CFGBuilder::Visit(const frontEnd::ASTIfStmt *Stmt) const {
  CFGBlock *ConditionBlock = CreateBlock("Branch");
  CFGBlock *ThenBlock = CreateBlock("Then");
  CFGBlock *MergeBlock = CreateBlock("IfContinue");
  CFGBlock *ElseBlock = nullptr;

  CFGBlock::AddLink(CurrentBlock, ConditionBlock);
  CurrentBlock = ConditionBlock;

  if (!Stmt->GetElseBody())
    CreateConditionalBranch(Stmt->GetCondition().get(), ThenBlock, MergeBlock);
  else {
    ElseBlock = CreateBlock("Else");
    CreateConditionalBranch(Stmt->GetCondition().get(), ThenBlock, ElseBlock);
  }

  CurrentBlock = ThenBlock;
  Traverse(Stmt->GetThenBody().get());

  if (Stmt->GetElseBody()) {
    CFGBlock::AddLink(ThenBlock, MergeBlock);
    CFGBlock::AddLink(ElseBlock, MergeBlock);
    CFGBlock::AddLink(ConditionBlock, ElseBlock);
    CurrentBlock = ElseBlock;
    Traverse(Stmt->GetElseBody().get());
  } else {
    CFGBlock::AddLink(CurrentBlock, MergeBlock);
  }

  CurrentBlock = MergeBlock;
}

void CFGBuilder::Visit(const frontEnd::ASTWhileStmt *Stmt) const {
  CFGBlock *WhileBodyBlock = CreateBlock("WhileBody");
  CFGBlock *MergeBlock = CreateBlock("WhileContinue");

  CreateConditionalBranch(Stmt->GetCondition().get(), WhileBodyBlock,
                          MergeBlock);
  CFGBlock *EntryBlock = CurrentBlock;

  CurrentBlock = WhileBodyBlock;
  Traverse(Stmt->GetBody().get());

  CurrentBlock->AddStatement(std::make_unique<IRBranch>(
      Stmt->GetCondition().get(), EntryBlock, MergeBlock));

  CFGBlock::AddLink(WhileBodyBlock, MergeBlock);
  CFGBlock::AddLink(WhileBodyBlock, EntryBlock);

  CurrentBlock = MergeBlock;
}

void CFGBuilder::CommitSSAFormBuilding() {
  CFGraph->CommitAllChanges();
  InsertPhiNodes();
  BuildSSAForm();
}

void CFGBuilder::InsertPhiNodes() {
  for (const auto &[VariableName, AssignedBlocks] : BlocksForVariable) {
    std::set<CFGBlock *> DominanceFrontier =
        CFGraph->GetDominanceFrontierForSubset(AssignedBlocks);
    for (auto *Block : DominanceFrontier) {
      std::map<CFGBlock *, ASTSymbol *> ToVarMap;
      for (auto *Predecessor : Block->Predecessors)
        ToVarMap[Predecessor] = new ASTSymbol(VariableName);
      auto Phi = std::make_unique<IRPhiNode>(
          std::make_unique<ASTSymbol>(VariableName), ToVarMap);
      Block->Statements.insert(Block->Statements.begin(), std::move(Phi));
    }
  }
}

void CFGBuilder::BuildSSAForm() {
  SSAForm Builder(CFGraph.get());
  for (const auto &[Variable, _] : BlocksForVariable)
    Builder.VariableToSSA(Variable);
}

CFG *CFGBuilder::GetCFG() const { return CFGraph.get(); }

} // namespace middleEnd
} // namespace weak