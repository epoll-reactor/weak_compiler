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

/*!
 * Holds ownership of true and false branches, views AST condition node.
 */
class IRBranch : public IRNode {
public:
  IRBranch(frontEnd::ASTNode *TheConditionView, CFGBlock *TheTrueBranch,
           CFGBlock *TheFalseBranch);
  IRBranch(CFGBlock *Block);

  std::string Dump() const override;

  void Accept(IRVisitor *) override;

  bool IsConditional;
  frontEnd::ASTNode *ConditionView;
  CFGBlock *TrueBranch;
  CFGBlock *FalseBranch;

private:
  IRBranch(frontEnd::ASTNode *Cond, CFGBlock *TheTrueBranch,
           CFGBlock *TheFalseBranch, bool IsCond);
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_IR_IR_BRANCH_HPP
