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
static void CheckForOperand(const Instruction::AnyOperand &Instruction,
                            const char *ErrorMsg) {
  if (!std::holds_alternative<T>(Instruction)) {
    weak::DiagnosticError() << ErrorMsg;
  }
}

static void DumpTo(std::ostringstream &Stream,
                   const Instruction::AnyOperand &Operand) {
  auto OperandVisitor = [&](auto &&Arg) {
    using T = std::decay_t<decltype(Arg)>;

    if constexpr (std::is_same_v<T, signed>)
      Stream << Arg;
    else if constexpr (std::is_same_v<T, double>)
      Stream << Arg;
    else if constexpr (std::is_same_v<T, bool>)
      Stream << Arg;
    else if constexpr (std::is_same_v<T, InstructionReference>)
      Stream << std::string{"t" + std::to_string(Arg.LabelNo)};
  };

  std::visit(OperandVisitor, Operand);
}

namespace weak {
namespace middleEnd {

InstructionReference::InstructionReference(const Instruction &I)
    : LabelNo(I.GetLabelNo()) {}

InstructionReference::InstructionReference(const UnaryInstruction &I)
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

const Instruction::AnyOperand &Instruction::GetLeftInstruction() const {
  return LeftOperand;
}

signed Instruction::GetRightImm() const {
  CheckForOperand<signed>(RightOperand, "Right operand isn't an immediate.");
  return std::get<signed>(RightOperand);
}

const Instruction::AnyOperand &Instruction::GetRightInstruction() const {
  return RightOperand;
}

std::string Instruction::Dump() const {
  std::ostringstream Stream;
  std::string Label = "t" + std::to_string(LabelNo);

  Stream << Label;
  Stream << std::left << std::setw(4) << " = ";
  Stream << std::right << std::setw(8);

  DumpTo(Stream, LeftOperand);

  Stream << std::right << std::setw(4) << TokenToString(Operation) << ' ';
  Stream << std::right << std::setw(8);

  DumpTo(Stream, RightOperand);

  return Stream.str();
}

} // namespace middleEnd
} // namespace weak

namespace weak {
namespace middleEnd {

UnaryInstruction::UnaryInstruction(unsigned TheLabelNo,
                                   const AnyOperand &TheOperand)
    : LabelNo(TheLabelNo), Operand(TheOperand) {}

unsigned UnaryInstruction::GetLabelNo() const { return LabelNo; }

const UnaryInstruction::AnyOperand &UnaryInstruction::GetOperand() const {
  return Operand;
}

std::string UnaryInstruction::Dump() const {
  std::ostringstream Stream;
  std::string Label = "t" + std::to_string(LabelNo);

  Stream << Label;
  Stream << std::left << std::setw(4) << " = ";
  Stream << std::right << std::setw(8);

  DumpTo(Stream, Operand);
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

void IfInstruction::SetGotoLabel(unsigned Label) { GotoLabel = Label; }

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

  Stream << std::right << std::setw(11);

  DumpTo(Stream, LeftOperand);

  Stream << std::setw(4) << TokenToString(Operation);
  Stream << std::setw(9);

  DumpTo(Stream, RightOperand);

  Stream << " goto L" << GotoLabel;
  return Stream.str();
}

} // namespace middleEnd
} // namespace weak