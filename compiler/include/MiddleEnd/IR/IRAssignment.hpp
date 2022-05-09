/* IRAssignment.hpp - Definition of assignment instruction.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_IR_IR_ASSIGNMENT_HPP
#define WEAK_COMPILER_MIDDLE_END_IR_IR_ASSIGNMENT_HPP

#include "FrontEnd/AST/ASTSymbol.hpp"
#include "MiddleEnd/IR/IRNode.hpp"

namespace weak {
namespace middleEnd {

/// \brief Assignment instruction.
///
/// Holds ownership of its variable (actually symbol).
class IRAssignment : public IRNode {
public:
  IRAssignment(frontEnd::ASTNode *TheVariable,
               frontEnd::ASTNode *TheOperandView);

  ~IRAssignment();

  std::string Dump() const override;

  void Accept(IRVisitor *) override;

  frontEnd::ASTSymbol *GetVariable() const;
  frontEnd::ASTNode *GetOperand() const;

private:
  frontEnd::ASTNode *Variable;
  frontEnd::ASTNode *OperandView;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_IR_IR_ASSIGNMENT_HPP
