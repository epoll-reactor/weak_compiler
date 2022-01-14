#include "MiddleEnd/CodeGen/CodeEmitter.hpp"
#include "MiddleEnd/IR/Instruction.hpp"
#include "TestHelpers.hpp"

using namespace weak::middleEnd;

int main() {
  SECTION(IRBasic) {
    CodeEmitter E;
    E.Emit1Reg(Op::PUSH, Reg::REG_0);
    E.Emit2Reg(Op::MOV, Reg::REG_0, Reg::REG_1);
    E.Emit2Reg(Op::CMPEQ, Reg::REG_1, 0);
    E.Emit2Reg(Op::SHL, Reg::REG_2, Reg::REG_3);
    E.EmitJumpLabel(1);
    E.Emit1Reg(Op::INC, Reg::REG_4);
    E.Emit2Reg(Op::ADD, Reg::REG_0, 0xFFFFFF);
    E.Emit1Reg(Op::POP, Reg::REG_0);
    E.EmitJumpInstr(1);
    E.Dump();
  }
}