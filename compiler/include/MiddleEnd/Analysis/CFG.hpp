/* CFG.hpp - Control Flow Graph.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_ANALYSIS_CFG_HPP
#define WEAK_COMPILER_MIDDLE_END_ANALYSIS_CFG_HPP

#include "MiddleEnd/Analysis/CFGBlock.hpp"
#include <map>
#include <set>

namespace weak {
namespace middleEnd {

class CFG {
public:
  std::set<CFGBlock *>
  GetDominanceFrontierForSubset(const std::set<CFGBlock *> &);
  void CommitAllChanges();
  void AddBlock(CFGBlock *Block);

  std::vector<CFGBlock *> &GetBlocks();
  const std::vector<CFGBlock *> &GetBlocks() const;

private:
  void PredOrderDFS(CFGBlock *);
  void PostOrderDFS(CFGBlock *);
  void DomDFS(CFGBlock *);

  void ComputePredOrder();
  void ComputePostOrder();
  void ComputeDominatorTree();
  void ComputeBaseDominanceFrontier();
  std::set<CFGBlock *>
  GetMergedDominanceFrontierFromSubset(const std::set<CFGBlock *> &);

  std::vector<CFGBlock *> Blocks;
  std::vector<CFGBlock *> InPredOrder;
  std::vector<CFGBlock *> InPostOrder;
  std::map<CFGBlock *, int> VisitedMap;
  std::map<CFGBlock *, std::set<CFGBlock *>> DominanceFrontier;
};

std::string CFGToDot(CFG *);

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_ANALYSIS_CFG_HPP
