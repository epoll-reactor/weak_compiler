#include "MiddleEnd/IR/Instruction.hpp"
#include <iomanip>
#include <sstream>

namespace weak {
namespace middleEnd {

OneRegInstruction::OneRegInstruction(Op TheOp, Reg TheReg)
    : Operation(TheOp), Register(TheReg) {}

Op OneRegInstruction::GetOperation() const { return Operation; }

Reg OneRegInstruction::GetOperand() const { return Register; }

std::string OneRegInstruction::Dump() const {
  std::ostringstream stream;
  stream << std::left << std::setw(10) << OperationToString(Operation);
  stream << ' ' << RegisterToString(Register);
  return stream.str();
}

} // namespace middleEnd
} // namespace weak