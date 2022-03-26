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

class CFGBlock {
public:
  CFGBlock(int TheIndex, std::string TheLabel);

  std::string ToString() const;

  void AddStatement(std::unique_ptr<IRNode> &&);
  static void AddLink(CFGBlock *Predecessor, CFGBlock *Successor);

  std::vector<std::unique_ptr<IRNode>> Statements;
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