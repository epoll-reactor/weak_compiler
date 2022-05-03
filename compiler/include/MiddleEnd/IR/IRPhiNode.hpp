/* IRPhiNode.hpp - Definition of Phi node used in SSA form.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_IR_IR_PHI_NODE_HPP
#define WEAK_COMPILER_MIDDLE_END_IR_IR_PHI_NODE_HPP

#include "FrontEnd/AST/ASTSymbol.hpp"
#include "MiddleEnd/Analysis/CFGBlock.hpp"
#include "MiddleEnd/IR/IRNode.hpp"
#include <map>

namespace weak {
namespace middleEnd {

/*!
 * Phi node. Views its CFG blocks, holds ownership of its symbols.
 */
class IRPhiNode : public IRNode {
public:
  IRPhiNode(std::unique_ptr<frontEnd::ASTSymbol> &&TheVariable,
            std::map<CFGBlock *, frontEnd::ASTSymbol *> VarMap);

  ~IRPhiNode() override;

  std::string Dump() const override;

  void Accept(IRVisitor *) override {}

  std::unique_ptr<frontEnd::ASTSymbol> Variable;

  std::map<CFGBlock *, frontEnd::ASTSymbol *> VariableMap;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_IR_IR_PHI_NODE_HPP
