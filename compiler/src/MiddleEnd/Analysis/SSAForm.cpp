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
    switch (Stmt->Type) {
    case IRNode::PHI: {
      IRPhiNode *Phi = static_cast<IRPhiNode *>(Stmt);
      if (Phi->Variable->GetName() == Variable) {
        Phi->Variable->SetSSAIndex(CurrentSSAIndex);
        IndicesStack.push(CurrentSSAIndex++);
      }
      break;
    }
    case IRNode::BRANCH: {
      for (auto *Var : VariableSearcher.AllVarsUsedInStatement(Stmt))
        if (Var->GetName() == Variable)
          Var->SetSSAIndex(IndicesStack.top());
      break;
    }
    case IRNode::ASSIGN: {
      IRAssignment *Assignment = static_cast<IRAssignment *>(Stmt);
      if (Assignment->GetVariable()->GetName() == Variable) {
        Assignment->GetVariable()->SetSSAIndex(CurrentSSAIndex);
        IndicesStack.push(CurrentSSAIndex++);
      }
      break;
    }
    default:
      break;
    }
  }

  for (auto *Succ : Block->Successors) {
    for (const auto &Stmt : Succ->Statements) {
      if (Stmt->Type != IRNode::PHI)
        continue;
      auto *Phi = static_cast<IRPhiNode *>(Stmt);
      if (Phi->VariableMap.count(Block) &&
          Phi->VariableMap[Block]->GetName() == Variable &&
          !IndicesStack.empty())
        Phi->VariableMap[Block]->SetSSAIndex(IndicesStack.top());
    }
  }

  for (auto *Child : Block->DominatingBlocks)
    TraverseWithRespectToVariable(Child, Variable);

  for (const auto &Stmt : Block->Statements) {
    if (Stmt->Type != IRNode::ASSIGN)
      continue;
    IRAssignment *Assignment = static_cast<IRAssignment *>(Stmt);
    if (Assignment->GetVariable()->GetName() == Variable)
      IndicesStack.pop();
  }
}

} // namespace middleEnd
} // namespace weak