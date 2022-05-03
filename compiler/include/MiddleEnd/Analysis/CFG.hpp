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

/*!
 * \brief Control Flow Graph. Holds ownership of its blocks.
 */
class CFG {
public:
  /// Release all CFG blocks.
  ~CFG();

  /// Simply add block.
  void AddBlock(CFGBlock *);

  /// Get the dominance frontier for the set of blocks in which a variable
  /// was created or assigned to (this stuff if managed by CFG builder).
  /// Used to compute phi-nodes.
  std::set<CFGBlock *>
  GetDominanceFrontierForSubset(const std::set<CFGBlock *> &);

  /// Make pred-and-post-order traversals, compute the dominator tree and
  /// dominance frontier.
  void CommitAllChanges();

  std::vector<CFGBlock *> &GetBlocks();
  const std::vector<CFGBlock *> &GetBlocks() const;

private:
  /// Do pred-order traversal and save sequence of visited blocks
  /// in PredOrderResult.
  void PredOrderDFS(CFGBlock *);

  /// Do post-order traversal and save sequence of visited blocks
  /// in PostOrderResult.
  void PostOrderDFS(CFGBlock *);

  /// Make \ref VisitedMap of block successors.
  void DomDFS(CFGBlock *);

  /// Front-end for \ref PredOrderDFS.
  void ComputePredOrder();

  /// Front-end for \ref PostOrderDFS.
  void ComputePostOrder();

  /// Set block dominators via PredOrderResult.
  void ComputeDominatorTree();

  /// If you are forgot (like me) about dominance frontiers, here the great
  /// article:
  /// https://pages.cs.wisc.edu/~fischer/cs701.f05/lectures/Lecture22.pdf.
  void ComputeDominanceFrontier();

  /// Compute frontiers one by one block and return the merged
  /// into one set result.
  std::set<CFGBlock *>
  GetMergedDominanceFrontierFromSubset(const std::set<CFGBlock *> &);

  std::vector<CFGBlock *> Blocks;

  /// The list of blocks retrieved via pred-order traversal.
  std::vector<CFGBlock *> PredOrderResult;

  /// The list of blocks retrieved via post-order traversal.
  std::vector<CFGBlock *> PostOrderResult;

  /// Helper map to do traversals.
  std::map<CFGBlock *, bool> VisitedMap;

  std::map<CFGBlock *, std::set<CFGBlock *>> DominanceFrontier;
};

std::string CFGToDot(CFG *);

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_ANALYSIS_CFG_HPP
