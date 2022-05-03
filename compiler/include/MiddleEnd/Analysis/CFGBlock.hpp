/* CFGBlock.hpp - Control Flow Graph block.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_ANALYSIS_CFG_BLOCK_HPP
#define WEAK_COMPILER_MIDDLE_END_ANALYSIS_CFG_BLOCK_HPP

#include "MiddleEnd/IR/IRNode.hpp"
#include <memory>
#include <string>
#include <vector>

namespace weak {
namespace middleEnd {

/*!
 * Control Flow Graph node. Holds ownership of its statements.
 */
class CFGBlock {
public:
  CFGBlock(int TheIndex, std::string TheLabel);

  ~CFGBlock();

  std::string ToString() const;

  void AddStatement(IRNode *);
  static void AddLink(CFGBlock *Predecessor, CFGBlock *Successor);

  std::vector<IRNode *> Statements;
  std::vector<CFGBlock *> Successors;
  std::vector<CFGBlock *> Predecessors;

  CFGBlock *Dominator;
  std::vector<CFGBlock *> DominatingBlocks;

private:
  int Index;
  std::string Label;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_ANALYSIS_CFG_BLOCK_HPP