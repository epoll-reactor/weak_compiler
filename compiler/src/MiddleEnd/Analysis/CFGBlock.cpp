/* CFGBlock.cpp - Control Flow Graph block.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/Analysis/CFGBlock.hpp"
#include <algorithm>

namespace weak {
namespace middleEnd {

CFGBlock::CFGBlock(int TheIndex, std::string TheLabel)
    : Index(TheIndex), Label(std::move(TheLabel)) {}

CFGBlock::~CFGBlock() {
  for (IRNode *Statement : Statements)
    delete Statement;
}

std::string CFGBlock::GetName() const {
  return "CFG#" + std::to_string(Index) + "(" + Label + ")";
}

void CFGBlock::AddStatement(IRNode *Stmt) { Statements.push_back(Stmt); }

void CFGBlock::AddLink(CFGBlock *Predecessor, CFGBlock *Successor) {
  if (auto &PredRef = Successor->Predecessors;
      std::find(PredRef.begin(), PredRef.end(), Predecessor) == PredRef.end())
    PredRef.push_back(Predecessor);

  if (auto &SuccRef = Predecessor->Successors;
      std::find(SuccRef.begin(), SuccRef.end(), Successor) == SuccRef.end())
    SuccRef.push_back(Successor);
}

} // namespace middleEnd
} // namespace weak