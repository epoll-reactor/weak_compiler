/* IRVisitor.hpp - Common-use class to traverse IR statements.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_IR_IR_VISITOR_HPP
#define WEAK_COMPILER_MIDDLE_END_IR_IR_VISITOR_HPP

namespace weak {
namespace middleEnd {

class IRAssignment;
class IRBranch;

class IRVisitor {
public:
  virtual ~IRVisitor() = default;

  virtual void Visit(const IRAssignment *) const = 0;
  virtual void Visit(const IRBranch *) const = 0;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_IR_IR_VISITOR_HPP
