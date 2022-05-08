/* IRNode.hpp - Definition of basic IR node.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_IR_IR_NODE_HPP
#define WEAK_COMPILER_MIDDLE_END_IR_IR_NODE_HPP

#include "MiddleEnd/IR/IRVisitor.hpp"
#include <string>

namespace weak {
namespace middleEnd {

/// \brief Abstract instruction.
class IRNode {
public:
  enum NodeType { ASSIGN, BRANCH, PHI } Type;

  IRNode(NodeType TheType) : Type(TheType) {}

  virtual ~IRNode() = default;

  virtual std::string Dump() const = 0;

  virtual void Accept(IRVisitor *) = 0;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_IR_IR_NODE_HPP