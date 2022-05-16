/* IRBranch.cpp - Definition of branch instruction.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/IR/IRBranch.hpp"
#include "MiddleEnd/IR/IRNodePrinter.hpp"
#include <sstream>

namespace weak {
namespace middleEnd {

IRBranch::IRBranch(frontEnd::ASTNode *TheConditionView, CFGBlock *TheTrueBranch,
                   CFGBlock *TheFalseBranch)
    : IRBranch(TheConditionView, TheTrueBranch, TheFalseBranch, true) {}

IRBranch::IRBranch(frontEnd::ASTNode *TheConditionView, CFGBlock *TheTrueBranch,
                   CFGBlock *TheFalseBranch, bool IsCond)
    : IRNode(IRNode::BRANCH), IsConditional(IsCond),
      ConditionView(TheConditionView), TrueBranch(TheTrueBranch),
      FalseBranch(TheFalseBranch) {}

std::string IRBranch::Dump() const {
  std::ostringstream OutStream;
  OutStream << "Branch";

  if (IsConditional || ConditionView) {
    OutStream << "(";
    IRNodePrinter Printer(ConditionView, OutStream);
    Printer.Print();
    OutStream << ")";
  }
  OutStream << " on true to " << TrueBranch->GetName();

  if (IsConditional && FalseBranch)
    OutStream << ", on false to " << FalseBranch->GetName();

  return OutStream.str();
}

void IRBranch::Accept(IRVisitor *Visitor) { Visitor->Visit(this); }

} // namespace middleEnd
} // namespace weak