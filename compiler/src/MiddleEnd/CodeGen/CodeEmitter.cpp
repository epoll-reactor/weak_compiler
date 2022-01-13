#include "MiddleEnd/CodeGen/CodeEmitter.hpp"
#include <iostream>

namespace weak {
namespace middleEnd {

CodeEmitter::CodeEmitter() : Instructions(), CurrentLabel(0U) {}

void CodeEmitter::Emit(frontEnd::TokenType Type,
                       const Instruction::OperandVariant &Left,
                       const Instruction::OperandVariant &Right) {
  Instructions.emplace_back(CurrentLabel, Type, Left, Right);
  ++CurrentLabel;
}

void CodeEmitter::Dump() {
  for (const Instruction &I : Instructions) {
    std::cout << I.Dump() << std::endl;
  }
}

} // namespace middleEnd
} // namespace weak