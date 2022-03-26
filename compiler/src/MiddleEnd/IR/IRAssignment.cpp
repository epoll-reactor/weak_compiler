/* IRAssignment.cpp - Definition of assignment instruction.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/IR/IRAssignment.hpp"
#include "MiddleEnd/IR/IRNodePrinter.hpp"
#include <cassert>
#include <sstream>

using namespace weak::frontEnd;

namespace weak {
namespace middleEnd {

IRAssignment::IRAssignment(ASTNode *TheVariable, ASTNode *TheOperand)
    : IRNode(IRNode::ASSIGN), Variable(static_cast<ASTSymbol *>(TheVariable)),
      Operand(TheOperand) {}

std::string IRAssignment::Dump() const {
  assert(Variable);
  assert(Operand);
  std::ostringstream OutStream;
  {
    IRNodePrinter Printer(Variable, OutStream);
    Printer.Print();
  }
  OutStream << " = ";
  {
    IRNodePrinter Printer(Operand, OutStream);
    Printer.Print();
  }

  return OutStream.str();
}

void IRAssignment::Accept(IRVisitor *Visitor) { Visitor->Visit(this); }

frontEnd::ASTSymbol *IRAssignment::GetVariable() const { return Variable; }

frontEnd::ASTNode *IRAssignment::GetOperand() const { return Operand; }

} // namespace middleEnd
} // namespace weak