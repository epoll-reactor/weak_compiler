/* CFG.cpp - Control Flow Graph.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/Analysis/CFG.hpp"

namespace weak {
namespace middleEnd {

void CFG::AddBlock(CFGBlock *Block) { Blocks.push_back(Block); }

std::vector<CFGBlock*>& CFG::GetBlocks() { return Blocks; }

const std::vector<CFGBlock*>& CFG::GetBlocks() const { return Blocks; }

void CFG::ComputePredOrder() {
  if (InPredOrder.empty()) {
    VisitedMap.clear();
    PredOrderDFS(Blocks.front());
  }
}

void CFG::PredOrderDFS(CFGBlock *Block) {
  VisitedMap[Block] = 1;
  InPredOrder.push_back(Block);
  for (auto *Next : Block->Successors)
    if (!VisitedMap[Next])
      PredOrderDFS(Next);
}

void CFG::ComputePostOrder() {
  if (InPostOrder.empty()) {
    VisitedMap.clear();
    PostOrderDFS(Blocks.front());
  }
}

void CFG::PostOrderDFS(CFGBlock *Block) {
  VisitedMap[Block] = 1;
  for (auto *Next : Block->Successors)
    if (!VisitedMap[Next])
      PostOrderDFS(Next);
  InPostOrder.push_back(Block);
}

void CFG::ComputeDominatorTree() {
  if (!InPredOrder.empty())
    ComputePredOrder();

  for (auto *It : InPredOrder) {
    VisitedMap.clear();
    VisitedMap[It] = 1;
    DomDFS(InPredOrder.front());
    for (auto *Jt : InPredOrder)
      if (!VisitedMap[Jt])
        Jt->Dominator = It;
  }

  for (auto *V : Blocks) {
    if (CFGBlock *Dominator = V->Dominator)
      Dominator->DominatingBlocks.push_back(V);
  }
}

void CFG::DomDFS(CFGBlock *Block) {
  if (!VisitedMap[Block]) {
    VisitedMap[Block] = 1;
    for (auto *Next : Block->Successors)
      if (!VisitedMap[Next])
        DomDFS(Next);
  }
}

void CFG::ComputeBaseDominanceFrontier() {
  if (!DominanceFrontier.empty())
    return;

  for (auto *Block : Blocks)
    DominanceFrontier[Block];

  for (auto *Block : InPostOrder) {
    for (auto *Successor : Block->Successors)
      if (Successor->Dominator != Block)
        DominanceFrontier.at(Block).insert(Successor);

    for (auto *XDominator : Block->DominatingBlocks)
      for (auto *Dominance : DominanceFrontier.at(XDominator))
        if (Dominance->Dominator != Block)
          DominanceFrontier.at(Block).insert(Dominance);
  }
}

std::set<CFGBlock *>
CFG::GetMergedDominanceFrontierFromSubset(const std::set<CFGBlock *> &Subset) {
  if (DominanceFrontier.empty())
    ComputeBaseDominanceFrontier();

  std::set<CFGBlock *> Merged;

  for (auto *Block : Subset) {
    std::set<CFGBlock *> Frontier = DominanceFrontier.at(Block);
    Merged.insert(Frontier.begin(), Frontier.end());
  }

  return Merged;
}

std::set<CFGBlock *>
CFG::GetDominanceFrontierForSubset(const std::set<CFGBlock *> &Subset) {
  std::set<CFGBlock *> Result;
  std::set<CFGBlock *> Frontier = GetMergedDominanceFrontierFromSubset(Subset);
  bool WasChanged = true;

  while (WasChanged) {
    WasChanged = false;
    Frontier.insert(Subset.begin(), Subset.end());
    Frontier = GetMergedDominanceFrontierFromSubset(Frontier);
    if (Result != Frontier) {
      Result = Frontier;
      WasChanged = true;
    }
  }

  return Result;
}

void CFG::CommitAllChanges() {
  ComputePredOrder();
  ComputePostOrder();
  ComputeDominatorTree();
  ComputeBaseDominanceFrontier();
}

} // namespace middleEnd
} // namespace weak

std::string weak::middleEnd::CFGToDot(CFG *Graph) {
  std::string OutGraph;

  OutGraph += "digraph G {\n";
  OutGraph += "  node[shape=box];\n";

  for (auto *Block : Graph->GetBlocks()) {
    for (auto *Successor : Block->Successors) {
      auto Dump = [](CFGBlock *B) {
        std::string Result;

        Result += B->ToString();
        Result += "\n";
        for (const auto &Stmt : B->Statements)
          Result += Stmt->Dump() + "\n";

        return Result;
      };

      OutGraph += "\t\"";
      OutGraph += Dump(Block);
      OutGraph += "\" -> \"";
      OutGraph += Dump(Successor);
      OutGraph += "\"\n";
    }
  }

  return OutGraph + "}\n";
}