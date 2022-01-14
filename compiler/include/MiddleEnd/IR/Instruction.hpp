/* Instruction.hpp - Definition of IR instruction.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_IR_INSTRUCTION_HPP
#define WEAK_COMPILER_MIDDLE_END_IR_INSTRUCTION_HPP

#include "FrontEnd/Lex/Token.hpp"
#include "MiddleEnd/IR/Operations.hpp"
#include "MiddleEnd/IR/Registers.hpp"
#include <functional>
#include <variant>

namespace weak {
namespace middleEnd {

/*!
 * This is a three-address code instruction representation.
 *
 * This instruction has format <ID> = <L> <OP> <R>.
 */
class Instruction {
public:
  using OperandVariant =
      std::variant<signed, Reg, std::reference_wrapper<Instruction>>;

  Instruction(unsigned TheLabelNo, frontEnd::TokenType TheOperation,
              const OperandVariant &TheLeft, const OperandVariant &TheRight);

  unsigned GetLabelNo() const;
  frontEnd::TokenType GetOp() const;

  bool IsLeftImm() const;
  bool IsLeftReg() const;
  bool IsLeftVar() const;

  bool IsRightImm() const;
  bool IsRightReg() const;
  bool IsRightVar() const;

  signed GetLeftImm() const;
  signed GetRightImm() const;
  Reg GetLeftReg() const;
  Reg GetRightReg() const;

  std::string Dump() const;

private:
  /// Used to identify the temporary variable and refer to it later.
  unsigned LabelNo;

  frontEnd::TokenType Operation;

  OperandVariant LeftOperand;
  OperandVariant RightOperand;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_IR_INSTRUCTION_HPP