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

void SSAForm::Compute(std::string_view Variable) {
  SSAIndex = 0;
  std::stack<int>().swap(IndicesStack);
  Compute(CFGraph->GetBlocks().front(), Variable);
}

void SSAForm::Compute(CFGBlock *Block, std::string_view Variable) {
  for (const auto &Stmt : Block->Statements) {
    switch (Stmt->Type) {
    case IRNode::PHI: {
      const auto &Symbol = (static_cast<IRPhiNode *>(Stmt))->Variable;
      if (Symbol->GetName() == Variable) {
        Symbol->SetSSAIndex(SSAIndex);
        IndicesStack.push(SSAIndex++);
      }
      break;
    }
    case IRNode::BRANCH: {
      for (auto *Symbol : VariableSearcher.AllVarsUsedInStatement(Stmt))
        if (Symbol->GetName() == Variable)
          Symbol->SetSSAIndex(IndicesStack.top());
      break;
    }
    case IRNode::ASSIGN: {
      auto *Symbol = (static_cast<IRAssignment *>(Stmt))->GetVariable();
      if (Symbol->GetName() == Variable) {
        Symbol->SetSSAIndex(SSAIndex);
        IndicesStack.push(SSAIndex++);
      }
      break;
    }
    default:
      break;
    }
  }

  for (auto *Successor : Block->Successors) {
    for (const auto &Stmt : Successor->Statements) {
      if (Stmt->Type != IRNode::PHI)
        continue;
      auto *PhiNode = static_cast<IRPhiNode *>(Stmt);
      if (PhiNode->VariableMap.count(Block) &&
          PhiNode->VariableMap[Block]->GetName() == Variable &&
          !IndicesStack.empty())
        PhiNode->VariableMap[Block]->SetSSAIndex(IndicesStack.top());
    }
  }

  for (auto *Child : Block->DominatingBlocks)
    Compute(Child, Variable);

  for (const auto &Stmt : Block->Statements) {
    if (Stmt->Type != IRNode::ASSIGN)
      continue;
    auto *Symbol = (static_cast<IRAssignment *>(Stmt))->GetVariable();
    if (Symbol->GetName() == Variable)
      IndicesStack.pop();
  }
}

} // namespace middleEnd
} // namespace weak