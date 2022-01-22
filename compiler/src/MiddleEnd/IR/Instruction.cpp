/* Instruction.cpp - Definition of IR instructions.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/IR/Instruction.hpp"
#include "Utility/Diagnostic.hpp"
#include "Utility/VariantOverload.hpp"
#include <iomanip>

using namespace weak::frontEnd;
using namespace weak::middleEnd;

static constexpr unsigned
ResolveInstructionSize(const Instruction::AnyOperand &Operand) {
  unsigned Size = 0U;
  // clang-format off
  std::visit(weak::Overload {
    [&Size](signed) { Size = sizeof(signed); },
    [&Size](double) { Size = sizeof(double); },
    [&Size](bool  ) { Size = sizeof(  bool); },
    [&Size](Reference
                 I) { Size = I.GetCapacity(); }
  }, Operand);
  // clang-format on
  return Size;
}

static void DumpTo(std::ostringstream &Stream,
                   const Instruction::AnyOperand &Operand) {
  // clang-format off
  std::visit(weak::Overload {
    [&Stream](signed    I) { Stream << I; },
    [&Stream](double    I) { Stream << I; },
    [&Stream](bool      I) { Stream << std::boolalpha << I; },
    [&Stream](Reference I) { Stream << std::string{"t" + std::to_string(I.GetLabelNo())}; }
  }, Operand);
  // clang-format on
}

namespace weak {
namespace middleEnd {

Reference::Reference(const Instruction &I) : LabelNo(I.GetLabelNo()) {
  ReservedCapacity = I.GetCapacity();
}

Reference::Reference(const UnaryInstruction &I) : LabelNo(I.GetLabelNo()) {
  ReservedCapacity = I.GetCapacity();
}

unsigned Reference::GetLabelNo() const { return LabelNo; }

unsigned Reference::GetCapacity() const { return ReservedCapacity; }

bool Reference::operator==(const Reference &RHS) const {
  return LabelNo == RHS.LabelNo && ReservedCapacity == RHS.ReservedCapacity;
}

bool Reference::operator!=(const Reference &RHS) const {
  return !(RHS == *this);
}

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

void Instruction::SetLabelNo(unsigned L) { LabelNo = L; }

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

bool Instruction::operator==(const Instruction &RHS) const {
  return LabelNo == RHS.LabelNo && ReservedCapacity == RHS.ReservedCapacity &&
         Operation == RHS.Operation && LeftOperand == RHS.LeftOperand &&
         RightOperand == RHS.RightOperand;
}

bool Instruction::operator!=(const Instruction &rhs) const {
  return !(rhs == *this);
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

void UnaryInstruction::SetLabelNo(unsigned L) { LabelNo = L; }

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

bool UnaryInstruction::operator==(const UnaryInstruction &RHS) const {
  return LabelNo == RHS.LabelNo && ReservedCapacity == RHS.ReservedCapacity &&
         Operand == RHS.Operand;
}

bool UnaryInstruction::operator!=(const UnaryInstruction &rhs) const {
  return !(rhs == *this);
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

void IfInstruction::SetGotoLabel(unsigned L) { GotoLabel = L; }

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

bool IfInstruction::operator==(const IfInstruction &RHS) const {
  return Operation == RHS.Operation && LeftOperand == RHS.LeftOperand &&
         RightOperand == RHS.RightOperand && GotoLabel == RHS.GotoLabel;
}

bool IfInstruction::operator!=(const IfInstruction &RHS) const {
  return !(RHS == *this);
}

} // namespace middleEnd
} // namespace weak

namespace weak {
namespace middleEnd {

Jump::Jump(unsigned TheLabelNo) : LabelNo(TheLabelNo) {}

void Jump::SetLabelNo(unsigned L) { LabelNo = L; }

unsigned Jump::GetLabel() const { return LabelNo; }

std::string Jump::Dump() const {
  return std::string(4, ' ') + "goto L" + std::to_string(LabelNo);
}

bool Jump::operator==(const Jump &RHS) const { return LabelNo == RHS.LabelNo; }

bool Jump::operator!=(const Jump &RHS) const { return !(RHS == *this); }

} // namespace middleEnd
} // namespace weak

namespace weak {
namespace middleEnd {

std::ostream &operator<<(std::ostream &Stream, const Instruction &I) {
  return Stream << I.Dump();
}
std::ostream &operator<<(std::ostream &Stream, const UnaryInstruction &I) {
  return Stream << I.Dump();
}
std::ostream &operator<<(std::ostream &Stream, const IfInstruction &I) {
  return Stream << I.Dump();
}
std::ostream &operator<<(std::ostream &Stream, const GotoLabel &I) {
  return Stream << I.Dump();
}
std::ostream &operator<<(std::ostream &Stream, const Jump &I) {
  return Stream << I.Dump();
}
std::ostream &operator<<(std::ostream &Stream, const AnyInstruction &I) {
  // clang-format off
  std::visit([&Stream](const auto &Instruction) {
    Stream << Instruction.Dump();
  }, I);
  // clang-format on
  return Stream;
}

} // namespace middleEnd
} // namespace weak