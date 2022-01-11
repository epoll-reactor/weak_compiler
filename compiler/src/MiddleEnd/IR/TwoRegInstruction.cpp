#include "MiddleEnd/IR/Instruction.hpp"
#include "Utility/Diagnostic.hpp"
#include <iomanip>
#include <sstream>

namespace weak {
namespace middleEnd {

TwoRegInstruction::TwoRegInstruction(Op TheOp, Reg TheReg1, Reg TheReg2)
    : Operation(TheOp), Reg1(TheReg1), Reg2(TheReg2), Imm(0),
      RegOrImm(IsRegOrImm::REG) {}

TwoRegInstruction::TwoRegInstruction(Op TheOp, Reg TheReg1, signed TheImm)
    : Operation(TheOp), Reg1(TheReg1), Imm(TheImm), RegOrImm(IsRegOrImm::IMM) {}

Op TwoRegInstruction::GetOp() const { return Operation; }

Reg TwoRegInstruction::GetReg1() const { return Reg1; }

Reg TwoRegInstruction::GetReg2() const {
  if (RegOrImm != IsRegOrImm::REG) {
    DiagnosticError(0U, 0U) << "Second register is not set.";
    UnreachablePoint();
  }
  return Reg2;
}

signed TwoRegInstruction::GetImm() const {
  if (RegOrImm != IsRegOrImm::IMM) {
    DiagnosticError(0U, 0U) << "Second immediate is not set.";
    UnreachablePoint();
  }
  return Imm;
}

TwoRegInstruction::IsRegOrImm TwoRegInstruction::GetRegOrImm() const {
  return RegOrImm;
}

std::string TwoRegInstruction::Dump() const {
  std::ostringstream stream;
  stream << std::left << std::setw(10) << OperationToString(Operation);
  stream << ' ' << std::left << std::setw(7) << RegisterToString(Reg1);
  stream << ' ' << std::left << std::setw(7);
  if (RegOrImm == IsRegOrImm::IMM)
    stream << std::to_string(Imm);
  else
    stream << RegisterToString(Reg2);
  return stream.str();
}

} // namespace middleEnd
} // namespace weak