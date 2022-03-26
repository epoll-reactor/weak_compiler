#include "MiddleEnd/IR/IRPhiNode.hpp"
#include "FrontEnd/AST/ASTSymbol.hpp"
#include <sstream>

namespace weak {
namespace middleEnd {

IRPhiNode::IRPhiNode(std::unique_ptr<frontEnd::ASTSymbol> &&TheVariable,
                     std::map<CFGBlock *, frontEnd::ASTSymbol *> VarMap)
    : IRNode(IRNode::PHI), Variable(std::move(TheVariable)),
      BlockToVarMap(std::move(VarMap)) {}

std::string IRPhiNode::Dump() const {
  std::string Result;

  for (const auto &[Block, Symbol] : BlockToVarMap)
    Result += Block->ToString() + ":" + Symbol->GetName() + ", ";

  if (!Result.empty()) {
    Result.pop_back();
    Result.pop_back();
  }

  return Variable->GetName() + " = φ(" + Result + ")";
}

void IRPhiNode::Accept(IRVisitor *) {}

} // namespace middleEnd
} // namespace weak