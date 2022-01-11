#ifndef WEAK_COMPILER_MIDDLE_END_CODE_EMITTER_HPP
#define WEAK_COMPILER_MIDDLE_END_CODE_EMITTER_HPP

#include "MiddleEnd/IR/Instruction.hpp"
#include "MiddleEnd/IR/Operations.hpp"
#include "MiddleEnd/IR/Registers.hpp"
#include <vector>

namespace weak {
namespace middleEnd {

/// This is used to simplify the formation of program represented in IR.
class CodeEmitter {
public:
  void Emit2Reg(Op, Reg, Reg);
  void Emit2Reg(Op, Reg, signed);

  void Emit1Reg(Op, Reg);

  void EmitJumpInstr(signed Label);
  void EmitJumpLabel(signed Label);

  void Dump();

private:
  std::vector<Instruction> Instructions;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_CODE_EMITTER_HPP