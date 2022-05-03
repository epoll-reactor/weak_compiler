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

IRAssignment::IRAssignment(frontEnd::ASTNode *TheVariable,
                           ASTNode *TheOperandView)
    : IRNode(IRNode::ASSIGN), Variable(TheVariable),
      OperandView(TheOperandView) {}

IRAssignment::~IRAssignment() { delete Variable; }

std::string IRAssignment::Dump() const {
  assert(Variable);
  assert(OperandView);
  std::ostringstream OutStream;
  {
    IRNodePrinter Printer(Variable, OutStream);
    Printer.Print();
  }
  OutStream << " = ";
  {
    IRNodePrinter Printer(OperandView, OutStream);
    Printer.Print();
  }

  return OutStream.str();
}

void IRAssignment::Accept(IRVisitor *Visitor) { Visitor->Visit(this); }

frontEnd::ASTSymbol *IRAssignment::GetVariable() const {
  return static_cast<ASTSymbol *>(Variable);
}

frontEnd::ASTNode *IRAssignment::GetOperand() const { return OperandView; }

} // namespace middleEnd
} // namespace weak