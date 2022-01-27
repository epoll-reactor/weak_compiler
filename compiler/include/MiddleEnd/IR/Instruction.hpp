/* Instruction.hpp - Definition of IR instructions.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_IR_INSTRUCTION_HPP
#define WEAK_COMPILER_MIDDLE_END_IR_INSTRUCTION_HPP

#include "FrontEnd/Lex/Token.hpp"
#include <list>
#include <variant>

namespace weak {
namespace middleEnd {

class Instruction;
class UnaryInstruction;

/// Reference, used as instruction operand.
class Reference {
public:
  Reference(const Instruction &);
  Reference(const UnaryInstruction &);

  unsigned GetLabelNo() const;

  /// Get the size of referred unary/binary instruction in bytes.
  unsigned GetCapacity() const;

  std::string Dump() const;

  bool operator==(const Reference &RHS) const;
  bool operator!=(const Reference &RHS) const;

private:
  unsigned LabelNo;

  /// The size of referred instruction.
  unsigned ReservedCapacity;
};

/// Three-address instruction in format <ID> = <L> <OP> <R>.
class Instruction {
public:
  using AnyOperand = std::variant<signed, double, bool, Reference>;

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
  GotoLabel(unsigned TheLabelNo);

  unsigned GetLabel() const;

  std::string Dump() const;

  bool operator==(const GotoLabel &RHS) const;
  bool operator!=(const GotoLabel &RHS) const;

private:
  unsigned LabelNo;
};

/// This is jump instruction represented in format goto <LABEL_NO>.
class Jump {
public:
  Jump(unsigned TheLabelNo);

  void SetLabelNo(unsigned L);
  unsigned GetLabel() const;

  std::string Dump() const;

  bool operator==(const Jump &RHS) const;
  bool operator!=(const Jump &RHS) const;

private:
  unsigned LabelNo;
};

class Call {
public:
  Call(std::string TheName, std::list<Reference> &&TheArguments);

  const std::string &GetName() const;
  const std::list<Reference> &GetArguments() const;

  std::string Dump() const;

  bool operator==(const Call& RHS) const;
  bool operator!=(const Call& RHS) const;

private:
  std::string Name;
  std::list<Reference> Arguments;
};

using AnyInstruction =
  std::variant<Instruction, UnaryInstruction, IfInstruction, GotoLabel, Jump, Call>;

class FunctionBlock {
public:
  void SetName(std::string TheName);

  template <typename IIterator>
  void SetArguments(IIterator Begin, IIterator End) {
    static_assert(
      std::is_same_v<std::decay_t<decltype(*Begin)>, frontEnd::TokenType>);
    Arguments = std::list(Begin, End);
  }

  template <typename IIterator> void SetBody(IIterator Begin, IIterator End) {
    static_assert(
      std::is_same_v<std::decay_t<decltype(*Begin)>, AnyInstruction>);
    Body = std::list(Begin, End);
  }

  const std::string &GetName() const;
  const std::list<frontEnd::TokenType> &GetArguments() const;
  const std::list<AnyInstruction> &GetBody() const;

  std::string Dump() const;

  bool operator==(const FunctionBlock &RHS) const;
  bool operator!=(const FunctionBlock &RHS) const;

private:
  std::list<frontEnd::TokenType> Arguments;
  std::list<AnyInstruction> Body;
  std::string Name;
};

std::ostream &operator<<(std::ostream &, const weak::middleEnd::Instruction &);
std::ostream &operator<<(std::ostream &,
                         const weak::middleEnd::UnaryInstruction &);
std::ostream &operator<<(std::ostream &,
                         const weak::middleEnd::IfInstruction &);
std::ostream &operator<<(std::ostream &, const weak::middleEnd::GotoLabel &);
std::ostream &operator<<(std::ostream &, const weak::middleEnd::Jump &);
std::ostream &operator<<(std::ostream &,
                         const weak::middleEnd::AnyInstruction &);
std::ostream &operator<<(std::ostream &,
                         const weak::middleEnd::FunctionBlock &);
std::ostream &operator<<(std::ostream &,
                         const weak::middleEnd::Call &);

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_IR_INSTRUCTION_HPP