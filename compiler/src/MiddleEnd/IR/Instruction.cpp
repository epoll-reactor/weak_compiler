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

namespace {

template <class... Ts> struct Overload : Ts... { using Ts::operator()...; };
template <class... Ts> Overload(Ts...) -> Overload<Ts...>;

} // namespace

template <typename T>
static void CheckForOperand(const Instruction::AnyOperand &Instruction,
                            const char *ErrorMsg) {
  if (!std::holds_alternative<T>(Instruction)) {
    weak::DiagnosticError() << ErrorMsg;
  }
}

static constexpr unsigned
ResolveInstructionSize(const Instruction::AnyOperand &Operand) {
  unsigned Size = 0U;
  // clang-format off
  std::visit(Overload {
    [&Size](signed) { Size = sizeof(signed); },
    [&Size](double) { Size = sizeof(double); },
    [&Size](bool  ) { Size = sizeof(  bool); },
    [&Size](InstructionReference
                 I) { Size = I.GetCapacity(); }
  }, Operand);
  // clang-format on
  return Size;
}

static void DumpTo(std::ostringstream &Stream,
                   const Instruction::AnyOperand &Operand) {
  // clang-format off
  std::visit(Overload {
    [&Stream](signed I) { Stream << I; },
    [&Stream](double I) { Stream << I; },
    [&Stream](bool   I) { Stream << std::boolalpha << I; },
    [&Stream](InstructionReference
                     I) { Stream << std::string{"t" + std::to_string(I.GetLabelNo())}; }
  }, Operand);
  // clang-format on
}

namespace weak {
namespace middleEnd {

InstructionReference::InstructionReference(const Instruction &I)
    : LabelNo(I.GetLabelNo()) {
  ReservedCapacity = I.GetCapacity();
}

InstructionReference::InstructionReference(const UnaryInstruction &I)
    : LabelNo(I.GetLabelNo()) {
  ReservedCapacity = I.GetCapacity();
}

unsigned InstructionReference::GetLabelNo() const { return LabelNo; }

unsigned InstructionReference::GetCapacity() const { return ReservedCapacity; }

} // namespace middleEnd
} // namespace weak

namespace weak {
namespace middleEnd {

Instruction::Instruction(unsigned TheLabelNo, frontEnd::TokenType TheOperation,
                         const AnyOperand &TheLeft, const AnyOperand &TheRight)
    : LabelNo(TheLabelNo), Operation(TheOperation), LeftOperand(TheLeft),
      RightOperand(TheRight) {
  ReservedCapacity = std::max(ResolveInstructionSize(LeftOperand),
                              ResolveInstructionSize(RightOperand));
}

unsigned Instruction::GetLabelNo() const { return LabelNo; }

unsigned Instruction::GetCapacity() const { return ReservedCapacity; }

TokenType Instruction::GetOp() const { return Operation; }

const Instruction::AnyOperand &Instruction::GetLeftInstruction() const {
  return LeftOperand;
}

const Instruction::AnyOperand &Instruction::GetRightInstruction() const {
  return RightOperand;
}

std::string Instruction::Dump() const {
  std::ostringstream Stream;
  std::string Label = "t" + std::to_string(LabelNo);

  Stream << std::right << std::setw(6) << Label;
  Stream << std::right << std::setw(4) << ReservedCapacity << " bytes ";
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
    : LabelNo(TheLabelNo), Operand(TheOperand) {
  ReservedCapacity = ResolveInstructionSize(Operand);
}

unsigned UnaryInstruction::GetLabelNo() const { return LabelNo; }

const UnaryInstruction::AnyOperand &UnaryInstruction::GetOperand() const {
  return Operand;
}

unsigned UnaryInstruction::GetCapacity() const { return ReservedCapacity; }

std::string UnaryInstruction::Dump() const {
  std::ostringstream Stream;
  std::string Label = "t" + std::to_string(LabelNo);

  Stream << std::right << std::setw(6) << Label;
  Stream << std::right << std::setw(4) << ReservedCapacity << " bytes ";
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
  Stream << "if";

  Stream << std::right << std::setw(27);

  DumpTo(Stream, LeftOperand);

  Stream << std::setw(4) << TokenToString(Operation);
  Stream << std::setw(9);

  DumpTo(Stream, RightOperand);

  Stream << " goto L" << GotoLabel;
  return Stream.str();
}

} // namespace middleEnd
} // namespace weak