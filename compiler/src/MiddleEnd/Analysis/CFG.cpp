/* CFG.cpp - Control Flow Graph.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/Analysis/CFG.hpp"

namespace weak {
namespace middleEnd {

CFG::~CFG() {
  for (CFGBlock *Block : Blocks)
    delete Block;
}

void CFG::AddBlock(CFGBlock *Block) { Blocks.push_back(Block); }

std::vector<CFGBlock *> &CFG::GetBlocks() { return Blocks; }

const std::vector<CFGBlock *> &CFG::GetBlocks() const { return Blocks; }

void CFG::PredOrderDFS(CFGBlock *Block) {
  VisitedMap[Block] = 1;
  PredOrderResult.push_back(Block);
  for (auto *Next : Block->Successors)
    if (!VisitedMap[Next])
      PredOrderDFS(Next);
}

void CFG::ComputePredOrder() {
  if (PredOrderResult.empty()) {
    VisitedMap.clear();
    PredOrderDFS(Blocks.front());
  }
}

void CFG::PostOrderDFS(CFGBlock *Block) {
  VisitedMap[Block] = 1;
  for (auto *Next : Block->Successors)
    if (!VisitedMap[Next])
      PostOrderDFS(Next);
  PostOrderResult.push_back(Block);
}

void CFG::ComputePostOrder() {
  if (PostOrderResult.empty()) {
    VisitedMap.clear();
    PostOrderDFS(Blocks.front());
  }
}

void CFG::DomDFS(CFGBlock *Block) {
  if (VisitedMap[Block])
    return;

  VisitedMap[Block] = true;
  for (auto *Next : Block->Successors)
    if (!VisitedMap[Next])
      DomDFS(Next);
}

void CFG::ComputeDominatorTree() {
  if (!PredOrderResult.empty())
    ComputePredOrder();

  for (auto *It : PredOrderResult) {
    VisitedMap.clear();
    VisitedMap[It] = true;
    // Scan first of pred-order result successors.
    DomDFS(PredOrderResult.front());
    for (auto *Jt : PredOrderResult)
      // If our block is not successor of pred-order result block,
      // then It is dominator of Jt.
      //    (It)
      //     |
      //     v
      //    (Jt)
      //     |
      //     It is dominator of Jt
      if (!VisitedMap[Jt])
        Jt->Dominator = It;
  }

  for (const auto &Block : Blocks)
    if (CFGBlock *Dominator = Block->Dominator)
      Dominator->DominatingBlocks.push_back(Block);
}

void CFG::ComputeDominanceFrontier() {
  if (!DominanceFrontier.empty())
    return;

  for (const auto &Block : Blocks)
    // Just insert a pair of block and empty set.
    DominanceFrontier[Block];

  for (auto *Block : PostOrderResult) {
    for (auto *Successor : Block->Successors)
      if (Successor->Dominator != Block)
        // We need all block successors, expect those we dominate.
        DominanceFrontier.at(Block).insert(Successor);

    for (auto *Dominance : Block->DominatingBlocks)
      for (auto *XDominance : DominanceFrontier.at(Dominance))
        if (XDominance->Dominator != Block)
          DominanceFrontier.at(Block).insert(XDominance);
  }
}

std::set<CFGBlock *>
CFG::GetMergedDominanceFrontierFromSubset(const std::set<CFGBlock *> &Subset) {
  if (DominanceFrontier.empty())
    ComputeDominanceFrontier();

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
  ComputeDominanceFrontier();
}

} // namespace middleEnd
} // namespace weak

std::string weak::middleEnd::CFGToDot(CFG *Graph) {
  std::string OutGraph;

  OutGraph += "digraph G {\n";
  OutGraph += "  node[shape=box];\n";

  for (const auto &Block : Graph->GetBlocks()) {
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