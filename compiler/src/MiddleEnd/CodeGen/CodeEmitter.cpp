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
    // clang-format off
    std::visit(weak::Overload {
      [](const Instruction      &I) { std::cerr << I.Dump() << std::endl; },
      [](const UnaryInstruction &I) { std::cerr << I.Dump() << std::endl; },
      [](const IfInstruction    &I) { std::cerr << I.Dump() << std::endl; },
      [](const GotoLabel        &I) { std::cerr << I.Dump() << std::endl; },
      [](const Jump             &I) { std::cerr << I.Dump() << std::endl; }
    }, Any);
    // clang-format on
  }
}

namespace weak {
namespace middleEnd {

CodeEmitter::CodeEmitter() : Instructions(), CurrentLabel(0U) {}

Instruction *CodeEmitter::Emit(frontEnd::TokenType Type,
                               const Instruction::AnyOperand &Left,
                               const Instruction::AnyOperand &Right) {
  Instructions.emplace_back(Instruction(CurrentLabel, Type, Left, Right));
  ++CurrentLabel;
  Instruction *I = &std::get<Instruction>(Instructions.back());
  return I;
}

Instruction *CodeEmitter::Emit(const Instruction &Copy) {
  Instructions.push_back(Copy);
  ++CurrentLabel;
  Instruction *I = &std::get<Instruction>(Instructions.back());
  return I;
}

UnaryInstruction *CodeEmitter::Emit(const UnaryInstruction::AnyOperand &Op) {
  Instructions.emplace_back(UnaryInstruction(CurrentLabel, Op));
  ++CurrentLabel;
  UnaryInstruction *I = &std::get<UnaryInstruction>(Instructions.back());
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

IfInstruction *CodeEmitter::EmitIf(const Instruction &Instr,
                                   unsigned GotoLabel) {
  // clang-format off
  std::visit(Overload {
    [this, &Instr, &GotoLabel](const InstructionReference &L, const InstructionReference &R) {
      Instructions.emplace_back(
        IfInstruction(
          Instr.GetOp(), L, R, GotoLabel
        )
      );
    },
    [this, &Instr, &GotoLabel](const auto &L, const InstructionReference &R) {
      Instructions.emplace_back(
        IfInstruction(
          Instr.GetOp(), L, R, GotoLabel
        )
      );
    },
    [this, &Instr, &GotoLabel](const InstructionReference &L, const auto &R) {
      Instructions.emplace_back(
        IfInstruction(
          Instr.GetOp(), L, R, GotoLabel
        )
      );
    },
    [this, &Instr, &GotoLabel](const auto &L, const auto &R) {
      Instructions.emplace_back(
        IfInstruction(
          Instr.GetOp(), L, R, GotoLabel
        )
      );
    }
  }, Instr.GetLeftInstruction(), Instr.GetRightInstruction());
  // clang-format on
  IfInstruction *I = &std::get<IfInstruction>(Instructions.back());
  return I;
}

const GotoLabel *CodeEmitter::EmitGotoLabel(unsigned Label) {
  Instructions.emplace_back(GotoLabel(Label));
  GotoLabel *I = &std::get<GotoLabel>(Instructions.back());
  return I;
}

Jump *CodeEmitter::EmitJump(unsigned ToLabel) {
  Instructions.emplace_back(Jump(ToLabel));
  Jump *I = &std::get<Jump>(Instructions.back());
  return I;
}

void CodeEmitter::Dump() { DumpImpl(Instructions); }

void CodeEmitter::Dump(const std::list<AnyInstruction> &Instructions) {
  DumpImpl(Instructions);
}

void CodeEmitter::RemoveLast() {
  // clang-format off
  std::visit(Overload {
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