/* CodeEmitter.cpp - Helper class used to form IR.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

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