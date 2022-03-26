/* IRBranch.hpp - Definition of branch instruction.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_IR_IR_BRANCH_HPP
#define WEAK_COMPILER_MIDDLE_END_IR_IR_BRANCH_HPP

#include "MiddleEnd/Analysis/CFGBlock.hpp"
#include "MiddleEnd/IR/IRNode.hpp"

namespace weak {
namespace frontEnd {
class ASTNode;
} // namespace frontEnd
} // namespace weak

namespace weak {
namespace middleEnd {

class IRBranch : public IRNode {
public:
  IRBranch(frontEnd::ASTNode *Cond, CFGBlock *TrueBranch,
           CFGBlock *FalseBranch);
  IRBranch(CFGBlock *Block);

  std::string Dump() const override;

  void Accept(IRVisitor *) override;

  bool IsConditional;
  CFGBlock *FirstBranchBlock;
  CFGBlock *SecondBranchBlock;
  frontEnd::ASTNode *Condition;

private:
  IRBranch(frontEnd::ASTNode *Cond, CFGBlock *FirstBranch,
           CFGBlock *SecondBranch, bool IsCond);
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_IR_IR_BRANCH_HPP
