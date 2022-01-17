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

  unsigned GetLabelNo() const;

  /// Get the size of instruction in bytes.
  unsigned GetCapacity() const;

  frontEnd::TokenType GetOp() const;

  bool IsLeftImm() const;
  bool IsLeftVar() const;

  bool IsRightImm() const;
  bool IsRightVar() const;

  signed GetLeftImm() const;
  const AnyOperand &GetLeftInstruction() const;

  signed GetRightImm() const;
  const AnyOperand &GetRightInstruction() const;

  std::string Dump() const;

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

  unsigned GetLabelNo() const;

  /// Get the size of instruction in bytes.
  unsigned GetCapacity() const;

  const AnyOperand &GetOperand() const;

  std::string Dump() const;

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

private:
  unsigned LabelNo;
};

/// This is jump instruction represented in format goto <LABEL_NO>.
class Jump {
public:
  Jump(unsigned TheLabelNo) : LabelNo(TheLabelNo) {}

  unsigned GetLabel() const { return LabelNo; }

  std::string Dump() const { return "goto L" + std::to_string(LabelNo); }

private:
  unsigned LabelNo;
};

using AnyInstruction =
    std::variant<Instruction, UnaryInstruction, IfInstruction, GotoLabel, Jump>;

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_IR_INSTRUCTION_HPP