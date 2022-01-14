/* Instruction.cpp - Definition of IR instruction.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/IR/Instruction.hpp"
#include "Utility/Diagnostic.hpp"
#include <iomanip>
#include <sstream>

using namespace weak::frontEnd;
using namespace weak::middleEnd;

template <typename T>
void CheckForOperand(const Instruction::AnyOperand &Instruction,
                     const char *ErrorMsg) {
  if (!std::holds_alternative<T>(Instruction)) {
    weak::DiagnosticError() << ErrorMsg;
  }
}

namespace weak {
namespace middleEnd {

InstructionReference::InstructionReference(const Instruction &I)
    : LabelNo(I.GetLabelNo()) {}

Instruction::Instruction(unsigned TheLabelNo, frontEnd::TokenType TheOperation,
                         const AnyOperand &TheLeft, const AnyOperand &TheRight)
    : LabelNo(TheLabelNo), Operation(TheOperation), LeftOperand(TheLeft),
      RightOperand(TheRight) {}

unsigned Instruction::GetLabelNo() const { return LabelNo; }

TokenType Instruction::GetOp() const { return Operation; }

bool Instruction::IsLeftImm() const {
  return std::holds_alternative<signed>(LeftOperand);
}

bool Instruction::IsLeftVar() const {
  return std::holds_alternative<InstructionReference>(LeftOperand);
}

bool Instruction::IsRightImm() const {
  return std::holds_alternative<signed>(RightOperand);
}

bool Instruction::IsRightVar() const {
  return std::holds_alternative<InstructionReference>(RightOperand);
}

signed Instruction::GetLeftImm() const {
  CheckForOperand<signed>(LeftOperand, "Left operand isn't an immediate.");
  return std::get<signed>(LeftOperand);
}

signed Instruction::GetRightImm() const {
  CheckForOperand<signed>(RightOperand, "Right operand isn't an immediate.");
  return std::get<signed>(RightOperand);
}

std::string Instruction::Dump() const {
  std::ostringstream Stream;
  std::string Label = "t" + std::to_string(LabelNo);

  Stream << Label;
  Stream << std::left << std::setw(4) << " = ";
  Stream << std::right << std::setw(8);

  auto OperandVisitor = [&](auto &&Arg) {
    using T = std::decay_t<decltype(Arg)>;

    if constexpr (std::is_same_v<T, signed>)
      Stream << Arg;
    else if constexpr (std::is_same_v<T, InstructionReference>)
      Stream << std::string{"t" + std::to_string(Arg.LabelNo)};
  };

  std::visit(OperandVisitor, LeftOperand);

  Stream << std::right << std::setw(3) << TokenToString(Operation) << ' ';
  Stream << std::right << std::setw(8);

  std::visit(OperandVisitor, RightOperand);

  return Stream.str();
}

} // namespace middleEnd
} // namespace weak