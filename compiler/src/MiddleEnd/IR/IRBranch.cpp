#include "MiddleEnd/IR/IRBranch.hpp"
#include "MiddleEnd/IR/IRNodePrinter.hpp"
#include <sstream>

namespace weak {
namespace middleEnd {

IRBranch::IRBranch(frontEnd::ASTNode *Cond, CFGBlock *TrueBranch,
                   CFGBlock *FalseBranch)
    : IRBranch(Cond, TrueBranch, FalseBranch, true) {}

IRBranch::IRBranch(CFGBlock *Block)
    : IRBranch(nullptr, Block, nullptr, false) {}

IRBranch::IRBranch(frontEnd::ASTNode *Cond, CFGBlock *FirstBranch,
                   CFGBlock *SecondBranch, bool IsCond)
    : IRNode(IRNode::BRANCH), IsConditional(IsCond),
      FirstBranchBlock(FirstBranch), SecondBranchBlock(SecondBranch),
      Condition(Cond) {}

std::string IRBranch::Dump() const {
  std::ostringstream OutStream;
  OutStream << "Branch";

  if (IsConditional || Condition) {
    OutStream << "(";
    IRNodePrinter Printer(Condition, OutStream);
    Printer.Print();
    OutStream << ")";
  }
  OutStream << " on true to " << FirstBranchBlock->ToString();

  if (IsConditional && SecondBranchBlock) {
    OutStream << ", on false to " << SecondBranchBlock->ToString();
  }

  return OutStream.str();
}

void IRBranch::Accept(IRVisitor *Visitor) { Visitor->Visit(this); }

} // namespace middleEnd
} // namespace weak