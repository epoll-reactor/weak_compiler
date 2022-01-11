#include "MiddleEnd/IR/Instruction.hpp"

namespace weak {
namespace middleEnd {

JumpLabel::JumpLabel(signed TheLabelNo) : LabelNo(TheLabelNo) {}

signed JumpLabel::GetLabelNo() const { return LabelNo; }

std::string JumpLabel::Dump() const {
  return "L" + std::to_string(LabelNo) + ":";
}

} // namespace middleEnd
} // namespace weak