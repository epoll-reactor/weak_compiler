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

  void BeginFunction(std::string_view Name, std::list<frontEnd::TokenType> &&);
  void EndFunction();

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

  Call *EmitCall(std::string Name, std::list<Reference> &&Arguments);

  void RemoveLast();

  void Dump();
  static void Dump(const std::list<FunctionBlock> &);

  const AnyInstruction &GetLast() const;

  const std::list<AnyInstruction> &GetFunctionInstructions() const;
  const std::list<FunctionBlock> &GetFunctions() const;

private:
  std::list<frontEnd::TokenType> FunctionArguments;
  std::list<AnyInstruction> FunctionInstructions;
  std::list<FunctionBlock> Functions;
  std::string CurrentFunctionName;
  unsigned CurrentLabel;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_CODE_EMITTER_HPP