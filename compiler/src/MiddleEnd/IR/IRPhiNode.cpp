/* IRPhiNode.cpp - Definition of Phi node used in SSA form.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/IR/IRPhiNode.hpp"
#include "FrontEnd/AST/ASTSymbol.hpp"
#include <sstream>

namespace weak {
namespace middleEnd {

IRPhiNode::IRPhiNode(std::unique_ptr<frontEnd::ASTSymbol> &&TheVariable,
                     std::map<CFGBlock *, frontEnd::ASTSymbol *> VarMap)
    : IRNode(IRNode::PHI), Variable(std::move(TheVariable)),
      VariableMap(std::move(VarMap)) {}

IRPhiNode::~IRPhiNode() {
  for ([[maybe_unused]] auto [NodeView, Symbol] : VariableMap)
    delete Symbol;
}

std::string IRPhiNode::Dump() const {
  std::string Result;

  for (const auto &[Block, Symbol] : VariableMap)
    Result += Block->ToString() + ":" + Symbol->GetName() + ", ";

  if (!Result.empty()) {
    Result.pop_back();
    Result.pop_back();
  }

  return Variable->GetName() + " = φ(" + Result + ")";
}

} // namespace middleEnd
} // namespace weak