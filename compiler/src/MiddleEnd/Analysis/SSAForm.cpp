/* SSABuilder.cpp - Constructor of SSA form from CFG.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/Analysis/SSAForm.hpp"
#include "MiddleEnd/IR/IRAssignment.hpp"
#include "MiddleEnd/IR/IRBranch.hpp"
#include "MiddleEnd/IR/IRPhiNode.hpp"

namespace weak {
namespace middleEnd {

SSAForm::SSAForm(CFG *Graph) : CFGraph(Graph) {}

void SSAForm::VariableToSSA(std::string_view Variable) {
  CurrentSSAIndex = 0;
  std::stack<int>().swap(IndicesStack);
  TraverseWithRespectToVariable(CFGraph->GetBlocks().front(), Variable);
}

void SSAForm::TraverseWithRespectToVariable(CFGBlock *Block,
                                            std::string_view Variable) {
  for (const auto &Stmt : Block->Statements) {
    if (Stmt->Type == IRNode::PHI) {
      IRPhiNode *Phi = static_cast<IRPhiNode *>(Stmt.get());
      if (Phi->Variable->GetName() == Variable) {
        Phi->Variable->SetSSAIndex(CurrentSSAIndex);
        IndicesStack.push(CurrentSSAIndex++);
      }
    } else {
      for (auto *Var : VariableSearcher.AllVarsUsedInStatement(Stmt.get()))
        if (Var->GetName() == Variable)
          Var->SetSSAIndex(IndicesStack.top());
    }

    if (Stmt->Type == IRNode::ASSIGN) {
      IRAssignment *Assignment = static_cast<IRAssignment *>(Stmt.get());
      if (Assignment->GetVariable()->GetName() == Variable) {
        Assignment->GetVariable()->SetSSAIndex(CurrentSSAIndex);
        IndicesStack.push(CurrentSSAIndex++);
      }
    }
  }

  for (auto *Succ : Block->Successors) {
    for (const auto &Stmt : Succ->Statements) {
      if (Stmt->Type == IRNode::PHI) {
        auto *Phi = static_cast<IRPhiNode *>(Stmt.get());
        if (Phi->BlockToVarMap.count(Block) &&
            Phi->BlockToVarMap[Block]->GetName() == Variable && !IndicesStack.empty())
          Phi->BlockToVarMap[Block]->SetSSAIndex(IndicesStack.top());
      }
    }
  }

  for (auto *Child : Block->DominatingBlocks)
    TraverseWithRespectToVariable(Child, Variable);

  for (const auto &Stmt : Block->Statements) {
    if (Stmt->Type == IRNode::ASSIGN) {
      IRAssignment *Assignment = static_cast<IRAssignment *>(Stmt.get());
      if (Assignment->GetVariable()->GetName() == Variable)
        IndicesStack.pop();
    }
  }
}

} // namespace middleEnd
} // namespace weak