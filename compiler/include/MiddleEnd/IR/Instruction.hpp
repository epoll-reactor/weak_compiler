#ifndef WEAK_COMPILER_MIDDLE_END_IR_INSTRUCTION_HPP
#define WEAK_COMPILER_MIDDLE_END_IR_INSTRUCTION_HPP

#include "MiddleEnd/IR/Operations.hpp"
#include "MiddleEnd/IR/Registers.hpp"
#include <string>
#include <variant>

namespace weak {
namespace middleEnd {
/*! One-register instruction representation.
 *
 *  Supported operations: OPERATION REGISTER
 *
 *  +------+----------------------------------+
 *  | push | push register value to stack     |
 *  |  pop | get value from stack to register |
 *  |  inc | increment register value         |
 *  |  dec | decrement register value         |
 *  +------+----------------------------------+
 */
class OneRegInstruction {
public:
  OneRegInstruction(Op TheOp, Reg TheReg);

  Op GetOperation() const;
  Reg GetOperand() const;

  std::string Dump() const;

private:
  Op Operation;
  Reg Register;
};

/*! Two-register instruction representation.
 *
 *  Supported operations: OPERATION REGISTER1 REGISTER2
 *                        OPERATION REGISTER1       IMM
 *
 *  +--------+---------------------------------------------------+
 *  |    mov | move right register or immediate to left register |
 *  |    add | addition                                          |
 *  |    sub | subtraction                                       |
 *  |    mul | multiplication                                    |
 *  |    div | division                                          |
 *  |    and | bitwise AND                                       |
 *  |     or | bitwise OR                                        |
 *  |    shl | bitwise shift left                                |
 *  |    shr | bitwise shift right                               |
 *  |  cmpge | greater or equal comparison                       |
 *  |  cmple | less or equal comparison                          |
 *  |  cmpgt | greater comparison                                |
 *  |  cmplt | less comparison                                   |
 *  |  cmpeq | equality comparison                               |
 *  | cmpneq | not equal comparison                              |
 *  +--------+---------------------------------------------------+
 */
class TwoRegInstruction {
public:
  enum struct IsRegOrImm { REG, IMM };

  TwoRegInstruction(Op TheOp, Reg TheReg1, Reg TheReg2);
  TwoRegInstruction(Op TheOp, Reg TheReg1, signed TheImm);

  Op GetOp() const;
  Reg GetReg1() const;
  Reg GetReg2() const;
  signed GetImm() const;
  IsRegOrImm GetRegOrImm() const;

  std::string Dump() const;

private:
  Op Operation;
  Reg Reg1, Reg2;
  signed Imm;
  IsRegOrImm RegOrImm;
};

/// L$(NUMBER).
class JumpLabel {
public:
  JumpLabel(signed TheLabelNo);

  signed GetLabelNo() const;

  std::string Dump() const;

private:
  signed LabelNo;
};

/// jmp L$(NUMBER).
class JumpInstruction {
public:
  JumpInstruction(signed TheLabelNo);

  signed GetLabelNo() const;

  std::string Dump() const;

private:
  signed LabelNo;
};

using Instruction = std::variant<OneRegInstruction, TwoRegInstruction,
                                 JumpLabel, JumpInstruction>;

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_IR_INSTRUCTION_HPP
