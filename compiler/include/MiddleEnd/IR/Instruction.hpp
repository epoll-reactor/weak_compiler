/* Instruction.hpp - Definition of IR instructions.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_IR_INSTRUCTION_HPP
#define WEAK_COMPILER_MIDDLE_END_IR_INSTRUCTION_HPP

#include "FrontEnd/Lex/Token.hpp"
#include <functional>
#include <variant>

namespace weak {
namespace middleEnd {

class Instruction;
class UnaryInstruction;

/// Reference, used as instruction operand.
class InstructionReference {
public:
  InstructionReference(const Instruction &);
  InstructionReference(const UnaryInstruction &);

  unsigned GetLabelNo() const;

  /// Get the size of referred unary/binary instruction in bytes.
  unsigned GetCapacity() const;

  bool operator==(const InstructionReference &RHS) const;
  bool operator!=(const InstructionReference &RHS) const;

private:
  unsigned LabelNo;

  /// The size of referred instruction.
  unsigned ReservedCapacity;
};

/// Three-address instruction in format <ID> = <L> <OP> <R>.
class Instruction {
public:
  using AnyOperand = std::variant<signed, double, bool, InstructionReference>;

  Instruction(unsigned TheLabelNo, frontEnd::TokenType TheOperation,
              const AnyOperand &TheLeft, const AnyOperand &TheRight);

  void SetLabelNo(unsigned);
  unsigned GetLabelNo() const;

  /// Get the size of instruction in bytes.
  unsigned GetCapacity() const;

  frontEnd::TokenType GetOp() const;

  const AnyOperand &GetLeftInstruction() const;

  const AnyOperand &GetRightInstruction() const;

  std::string Dump() const;

  bool operator==(const Instruction &RHS) const;
  bool operator!=(const Instruction &RHS) const;

private:
  /// Used to identify the temporary variable and refer to it later.
  unsigned LabelNo;

  /// The instruction size in bytes.
  unsigned ReservedCapacity;

  frontEnd::TokenType Operation;

  AnyOperand LeftOperand;
  AnyOperand RightOperand;
};

/// Instruction in format <ID> = <VALUE>;
class UnaryInstruction {
public:
  using AnyOperand = Instruction::AnyOperand;

  UnaryInstruction(unsigned TheLabelNo, const AnyOperand &TheOperand);

  void SetLabelNo(unsigned);
  unsigned GetLabelNo() const;

  /// Get the size of instruction in bytes.
  unsigned GetCapacity() const;

  const AnyOperand &GetOperand() const;

  std::string Dump() const;

  bool operator==(const UnaryInstruction &RHS) const;
  bool operator!=(const UnaryInstruction &RHS) const;

private:
  unsigned LabelNo;

  /// The instruction size in bytes.
  unsigned ReservedCapacity;

  AnyOperand Operand;
};

/// Conditional instruction in format if <ARG> <OP> <ARG> goto L.
class IfInstruction {
public:
  using AnyOperand = Instruction::AnyOperand;

  IfInstruction(frontEnd::TokenType TheOperation, const AnyOperand &TheLeft,
                const AnyOperand &TheRight, unsigned TheGotoLabel);

  void SetGotoLabel(unsigned);

  unsigned GetGotoLabel() const;
  frontEnd::TokenType GetOperation() const;
  const AnyOperand &GetLeftOperand() const;
  const AnyOperand &GetRightOperand() const;

  std::string Dump() const;

  bool operator==(const IfInstruction &RHS) const;
  bool operator!=(const IfInstruction &RHS) const;

private:
  frontEnd::TokenType Operation;

  AnyOperand LeftOperand;
  AnyOperand RightOperand;

  unsigned GotoLabel;
};

/// Goto label in format L<NUMBER>:.
class GotoLabel {
public:
  GotoLabel(unsigned TheLabelNo) : LabelNo(TheLabelNo) {}

  unsigned GetLabel() const { return LabelNo; }

  std::string Dump() const { return "L" + std::to_string(LabelNo) + ":"; }

  bool operator==(const GotoLabel &RHS) const { return LabelNo == RHS.LabelNo; }
  bool operator!=(const GotoLabel &RHS) const { return !(RHS == *this); }

private:
  unsigned LabelNo;
};

/// This is jump instruction represented in format goto <LABEL_NO>.
class Jump {
public:
  Jump(unsigned TheLabelNo) : LabelNo(TheLabelNo) {}

  void SetLabelNo(unsigned L) { LabelNo = L; }
  unsigned GetLabel() const { return LabelNo; }

  std::string Dump() const { return "goto L" + std::to_string(LabelNo); }

  bool operator==(const Jump &RHS) const { return LabelNo == RHS.LabelNo; }

  bool operator!=(const Jump &RHS) const { return !(RHS == *this); }

private:
  unsigned LabelNo;
};

using AnyInstruction =
    std::variant<Instruction, UnaryInstruction, IfInstruction, GotoLabel, Jump>;

std::ostream &operator<<(std::ostream &, const weak::middleEnd::Instruction &);
std::ostream &operator<<(std::ostream &,
                         const weak::middleEnd::UnaryInstruction &);
std::ostream &operator<<(std::ostream &,
                         const weak::middleEnd::IfInstruction &);
std::ostream &operator<<(std::ostream &, const weak::middleEnd::GotoLabel &);
std::ostream &operator<<(std::ostream &, const weak::middleEnd::Jump &);
std::ostream &operator<<(std::ostream &,
                         const weak::middleEnd::AnyInstruction &);

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_IR_INSTRUCTION_HPP