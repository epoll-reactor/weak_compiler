/* CodeEmitter.cpp - Helper class used to form IR.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/CodeGen/CodeEmitter.hpp"
#include "Utility/VariantOverload.hpp"
#include <iostream>

using namespace weak::middleEnd;

static void DumpImpl(const std::list<AnyInstruction> &Instructions) {
  for (const AnyInstruction &Any : Instructions) {
    std::visit([](const auto &I) { std::cerr << I.Dump() << std::endl; }, Any);
  }
}

namespace weak {
namespace middleEnd {

CodeEmitter::CodeEmitter() : Instructions(), CurrentLabel(0U) {}

Instruction *CodeEmitter::Emit(frontEnd::TokenType Type,
                               const Instruction::AnyOperand &LHS,
                               const Instruction::AnyOperand &RHS) {
  Instruction *I = &std::get<Instruction>(
      Instructions.emplace_back(Instruction(CurrentLabel, Type, LHS, RHS)));
  ++CurrentLabel;
  return I;
}

Instruction *CodeEmitter::Emit(const Instruction &Copy) {
  ++CurrentLabel;
  return &std::get<Instruction>(Instructions.emplace_back(Copy));
}

UnaryInstruction *CodeEmitter::Emit(const UnaryInstruction::AnyOperand &Op) {
  UnaryInstruction *I = &std::get<UnaryInstruction>(
      Instructions.emplace_back(UnaryInstruction(CurrentLabel, Op)));
  ++CurrentLabel;
  return I;
}

IfInstruction *CodeEmitter::EmitIf(frontEnd::TokenType Operation,
                                   const Instruction::AnyOperand &Left,
                                   const Instruction::AnyOperand &Right,
                                   unsigned GotoLabel) {
  Instructions.emplace_back(IfInstruction(Operation, Left, Right, GotoLabel));
  IfInstruction *I = &std::get<IfInstruction>(Instructions.back());
  return I;
}

IfInstruction *CodeEmitter::EmitIf(const Instruction &Copy,
                                   unsigned GotoLabel) {
  // clang-format off
  std::visit(Overload{
    [&](const auto& LHS, const auto& RHS) {
      Instructions.emplace_back(
        IfInstruction(
          Copy.GetOp(), LHS, RHS, GotoLabel
        )
      );
    }
  }, Copy.GetLeftInstruction(), Copy.GetRightInstruction());
  // clang-format on
  IfInstruction *I = &std::get<IfInstruction>(Instructions.back());
  return I;
}

const GotoLabel *CodeEmitter::EmitGotoLabel(unsigned Label) {
  return &std::get<GotoLabel>(Instructions.emplace_back(GotoLabel(Label)));
}

Jump *CodeEmitter::EmitJump(unsigned Label) {
  return &std::get<Jump>(Instructions.emplace_back(Jump(Label)));
}

void CodeEmitter::Dump() { DumpImpl(Instructions); }

void CodeEmitter::Dump(const std::list<AnyInstruction> &Instructions) {
  DumpImpl(Instructions);
}

void CodeEmitter::RemoveLast() {
  // clang-format off
  std::visit(Overload{
    [this](const Instruction      &) { --CurrentLabel;   },
    [this](const UnaryInstruction &) { --CurrentLabel;   },
    [    ](const IfInstruction    &) { /* Do nothing. */ },
    [    ](const GotoLabel        &) { /* Do nothing. */ },
    [    ](const Jump             &) { /* Do nothing. */ }
  }, Instructions.back());
  // clang-format on
  Instructions.pop_back();
}

const AnyInstruction &CodeEmitter::GetLast() const {
  return Instructions.back();
}

const std::list<AnyInstruction> &CodeEmitter::GetInstructions() const {
  return Instructions;
}

} // namespace middleEnd
} // namespace weak