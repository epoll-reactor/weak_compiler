#include "MiddleEnd/IR/Instruction.hpp"
#include <iomanip>
#include <sstream>

namespace weak {
namespace middleEnd {

JumpInstruction::JumpInstruction(signed TheLabelNo) : LabelNo(TheLabelNo) {}

signed JumpInstruction::GetLabelNo() const { return LabelNo; }

std::string JumpInstruction::Dump() const {
  std::ostringstream stream;
  stream << std::left << std::setw(11) << "jmp"
         << "L" << LabelNo;
  return stream.str();
}

} // namespace middleEnd
} // namespace weak