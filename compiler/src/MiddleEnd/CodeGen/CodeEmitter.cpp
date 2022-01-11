#include "MiddleEnd/CodeGen/CodeEmitter.hpp"
#include <iostream>

namespace weak {
namespace middleEnd {

void CodeEmitter::Emit2Reg(Op Op, Reg Reg1, Reg Reg2) {
  Instructions.push_back(TwoRegInstruction(Op, Reg1, Reg2));
}

void CodeEmitter::Emit2Reg(Op Op, Reg Reg1, signed Imm) {
  Instructions.push_back(TwoRegInstruction(Op, Reg1, Imm));
}

void CodeEmitter::Emit1Reg(Op Op, Reg Reg) {
  Instructions.push_back(OneRegInstruction(Op, Reg));
}

void CodeEmitter::EmitJumpInstr(signed Label) {
  Instructions.push_back(JumpInstruction(Label));
}

void CodeEmitter::EmitJumpLabel(signed Label) {
  Instructions.push_back(JumpLabel(Label));
}

void CodeEmitter::Dump() {
  struct InstrVisitor {
    void operator()(const TwoRegInstruction &TwoReg) {
      std::cout << TwoReg.Dump() << std::endl;
    }
    void operator()(const OneRegInstruction &OneReg) {
      std::cout << OneReg.Dump() << std::endl;
    }
    void operator()(const JumpLabel &Jump) {
      std::cout << Jump.Dump() << std::endl;
    }
    void operator()(const JumpInstruction &Jump) {
      std::cout << Jump.Dump() << std::endl;
    }
  };

  for (const auto &I : Instructions) {
    std::visit(InstrVisitor{}, I);
  }
}

} // namespace middleEnd
} // namespace weak