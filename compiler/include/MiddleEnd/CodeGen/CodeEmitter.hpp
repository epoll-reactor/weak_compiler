/* CodeEmitter.hpp - Helper class used to form IR.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_CODE_EMITTER_HPP
#define WEAK_COMPILER_MIDDLE_END_CODE_EMITTER_HPP

#include "MiddleEnd/IR/Instruction.hpp"
#include <list>

namespace weak {
namespace middleEnd {

/// This is used to simplify the formation of program represented in IR.
class CodeEmitter {
public:
  CodeEmitter();

  Instruction *Emit(frontEnd::TokenType, const Instruction::AnyOperand &,
                    const Instruction::AnyOperand &);
  Instruction *Emit(const Instruction &);

  UnaryInstruction *Emit(const UnaryInstruction::AnyOperand &);

  IfInstruction *EmitIf(frontEnd::TokenType Operation,
                        const Instruction::AnyOperand &LHS,
                        const Instruction::AnyOperand &RHS, unsigned GotoLabel);
  IfInstruction *EmitIf(const Instruction &Copy, unsigned GotoLabel);

  const GotoLabel *EmitGotoLabel(unsigned Label);
  Jump *EmitJump(unsigned Label);

  void RemoveLast();

  void Dump();
  static void Dump(const std::list<AnyInstruction> &);

  const AnyInstruction &GetLast() const;
  const std::list<AnyInstruction> &GetInstructions() const;

private:
  std::list<AnyInstruction> Instructions;
  unsigned CurrentLabel;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_CODE_EMITTER_HPP