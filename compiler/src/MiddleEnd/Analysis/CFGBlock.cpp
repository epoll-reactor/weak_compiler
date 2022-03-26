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

std::string CFGBlock::ToString() const {
  return "CFG#" + std::to_string(Index) + "(" + Label + ")";
}

void CFGBlock::AddStatement(std::unique_ptr<IRNode> &&Stmt) {
  Statements.push_back(std::move(Stmt));
}

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