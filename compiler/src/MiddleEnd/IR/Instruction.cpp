#include "MiddleEnd/IR/Instruction.hpp"
#include "Utility/Diagnostic.hpp"
#include <iomanip>
#include <sstream>

using namespace weak::frontEnd;
using namespace weak::middleEnd;

template <typename T>
void CheckForOperand(const Instruction::OperandVariant &Instruction,
                     const char *ErrorMsg) {
  if (!std::holds_alternative<T>(Instruction)) {
    weak::DiagnosticError(0U, 0U) << ErrorMsg;
  }
}

namespace weak {
namespace middleEnd {

Instruction::Instruction(unsigned TheLabelNo, frontEnd::TokenType TheOperation,
                         const OperandVariant &TheLeft,
                         const OperandVariant &TheRight)
    : LabelNo(TheLabelNo), Operation(TheOperation), LeftOperand(TheLeft),
      RightOperand(TheRight) {}

unsigned Instruction::GetLabelNo() const { return LabelNo; }

TokenType Instruction::GetOp() const { return Operation; }

bool Instruction::IsLeftImm() const {
  return std::holds_alternative<signed>(LeftOperand);
}

bool Instruction::IsLeftReg() const {
  return std::holds_alternative<Reg>(LeftOperand);
}

bool Instruction::IsLeftVar() const {
  return std::holds_alternative<std::reference_wrapper<Instruction>>(
      LeftOperand);
}

bool Instruction::IsRightImm() const {
  return std::holds_alternative<signed>(RightOperand);
}

bool Instruction::IsRightReg() const {
  return std::holds_alternative<Reg>(LeftOperand);
}

bool Instruction::IsRightVar() const {
  return std::holds_alternative<std::reference_wrapper<Instruction>>(
      RightOperand);
}

signed Instruction::GetLeftImm() const {
  CheckForOperand<signed>(LeftOperand, "Left operand isn't an immediate.");
  return std::get<signed>(LeftOperand);
}

signed Instruction::GetRightImm() const {
  CheckForOperand<signed>(RightOperand, "Right operand isn't an immediate.");
  return std::get<signed>(RightOperand);
}

Reg Instruction::GetLeftReg() const {
  CheckForOperand<Reg>(LeftOperand, "Left operand isn't a register.");
  return std::get<Reg>(LeftOperand);
}

Reg Instruction::GetRightReg() const {
  CheckForOperand<Reg>(RightOperand, "Right operand isn't a register.");
  return std::get<Reg>(RightOperand);
}

std::string Instruction::Dump() const {
  std::ostringstream Stream;
  std::string Label = "t" + std::to_string(LabelNo);

  Stream << Label;
  Stream << std::left << std::setw(4) << " = ";
  Stream << std::right << std::setw(8);

  auto OperandVisitor = [&](auto &&Arg) {
    using T = std::decay_t<decltype(Arg)>;

    if constexpr (std::is_same_v<T, signed>)
      Stream << Arg;
    else if constexpr (std::is_same_v<T, Reg>)
      Stream << RegisterToString(Arg);
    else if constexpr (std::is_same_v<T, std::reference_wrapper<Instruction>>)
      Stream << std::string{"t" + std::to_string(Arg.get().LabelNo)};
  };

  std::visit(OperandVisitor, LeftOperand);

  Stream << std::right << std::setw(3) << TokenToString(Operation) << ' ';
  Stream << std::right << std::setw(8);

  std::visit(OperandVisitor, RightOperand);

  return Stream.str();
}

} // namespace middleEnd
} // namespace weak