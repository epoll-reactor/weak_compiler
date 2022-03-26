/* SSABuilder.cpp - Constructor of SSA form from CFG.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/Analysis/SSABuilder.hpp"
#include "MiddleEnd/IR/IRAssignment.hpp"
#include "MiddleEnd/IR/IRBranch.hpp"
#include "MiddleEnd/IR/IRPhiNode.hpp"

namespace weak {
namespace middleEnd {

SSABuilder::SSABuilder(CFG *Graph) : CFGraph(Graph) {}

void SSABuilder::VariableToSSA(std::string_view VarName) {
  Counter = 0;
  std::stack<int>().swap(Stack);
  TraverseWithRespectToVariable(CFGraph->BasicBlocks.front(), VarName);
}

void SSABuilder::TraverseWithRespectToVariable(CFGBlock *Block,
                                               std::string_view VarName) {
  for (const auto &Stmt : Block->Statements) {
    if (Stmt->Type == IRNode::PHI) {
      IRPhiNode *Phi = static_cast<IRPhiNode *>(Stmt.get());
      if (Phi->Variable->GetName() == VarName) {
        Phi->Variable->SetSSAIndex(Counter);
        Stack.push(Counter++);
      }
    } else {
      auto Vars = VarSearcher.AllVarsUsedInStatement(Stmt.get());
      for (auto *Var : Vars)
        if (Var->GetName() == VarName)
          Var->SetSSAIndex(Stack.top());
    }

    if (Stmt->Type == IRNode::ASSIGN) {
      IRAssignment *Assignment = static_cast<IRAssignment *>(Stmt.get());
      if (Assignment->GetVariable()->GetName() == VarName) {
        Assignment->GetVariable()->SetSSAIndex(Counter);
        Stack.push(Counter++);
      }
    }
  }

  for (auto *Succ : Block->Successors) {
    for (const auto &Stmt : Succ->Statements) {
      if (Stmt->Type == IRNode::PHI) {
        IRPhiNode *Phi = static_cast<IRPhiNode *>(Stmt.get());
        if (Phi->BlockToVarMap.count(Block) &&
            Phi->BlockToVarMap[Block]->GetName() == VarName && !Stack.empty())
          Phi->BlockToVarMap[Block]->SetSSAIndex(Stack.top());
      }
    }
  }

  for (auto *Child : Block->DominatingBlocks)
    TraverseWithRespectToVariable(Child, VarName);

  for (const auto &Stmt : Block->Statements) {
    if (Stmt->Type == IRNode::ASSIGN) {
      IRAssignment *Assignment = static_cast<IRAssignment *>(Stmt.get());
      if (Assignment->GetVariable()->GetName() == VarName)
        Stack.pop();
    }
  }
}

} // namespace middleEnd
} // namespace weak