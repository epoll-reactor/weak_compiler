/* Instruction.cpp - Definition of IR instructions.
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

} // namespace middleEnd
} // namespace weak

namespace weak {
namespace middleEnd {

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

InstructionReference Instruction::GetLeftInstruction() const {
  CheckForOperand<InstructionReference>(LeftOperand,
                                        "Left operand isn't an immediate.");
  return std::get<InstructionReference>(LeftOperand);
}

signed Instruction::GetRightImm() const {
  CheckForOperand<signed>(RightOperand, "Right operand isn't an immediate.");
  return std::get<signed>(RightOperand);
}

InstructionReference Instruction::GetRightInstruction() const {
  CheckForOperand<InstructionReference>(RightOperand,
                                        "Right operand isn't an immediate.");
  return std::get<InstructionReference>(RightOperand);
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
    if constexpr (std::is_same_v<T, double>)
      Stream << Arg;
    else if constexpr (std::is_same_v<T, InstructionReference>)
      Stream << std::string{"t" + std::to_string(Arg.LabelNo)};
  };

  std::visit(OperandVisitor, LeftOperand);

  Stream << std::right << std::setw(4) << TokenToString(Operation) << ' ';
  Stream << std::right << std::setw(8);

  std::visit(OperandVisitor, RightOperand);

  return Stream.str();
}

} // namespace middleEnd
} // namespace weak

namespace weak {
namespace middleEnd {

IfInstruction::IfInstruction(frontEnd::TokenType TheOperation,
                             const AnyOperand &TheLeft,
                             const AnyOperand &TheRight, unsigned TheGotoLabel)
    : Operation(TheOperation), LeftOperand(TheLeft), RightOperand(TheRight),
      GotoLabel(TheGotoLabel) {}

unsigned IfInstruction::GetGotoLabel() const { return GotoLabel; }

TokenType IfInstruction::GetOperation() const { return Operation; }

const IfInstruction::AnyOperand &IfInstruction::GetLeftOperand() const {
  return LeftOperand;
}

const IfInstruction::AnyOperand &IfInstruction::GetRightOperand() const {
  return RightOperand;
}

std::string IfInstruction::Dump() const {
  std::ostringstream Stream;
  Stream << "if ";

  auto Visitor = [&](auto &&Arg) {
    using T = std::decay_t<decltype(Arg)>;

    if constexpr (std::is_same_v<T, signed>)
      Stream << Arg;
    else if constexpr (std::is_same_v<T, InstructionReference>)
      Stream << std::string{"t" + std::to_string(Arg.LabelNo)};
  };

  Stream << std::right << std::setw(11);
  std::visit(Visitor, LeftOperand);
  Stream << std::setw(4) << TokenToString(Operation);
  Stream << std::setw(7) << " ";
  std::visit(Visitor, RightOperand);

  Stream << " goto L" << GotoLabel;
  return Stream.str();
}

} // namespace middleEnd
} // namespace weak