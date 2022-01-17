/* CodeEmitter.cpp - Helper class used to form IR.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/CodeGen/CodeEmitter.hpp"
#include <iostream>

namespace {

template <class... Ts> struct Overload : Ts... { using Ts::operator()...; };
template <class... Ts> Overload(Ts...) -> Overload<Ts...>;

} // namespace

namespace weak {
namespace middleEnd {

CodeEmitter::CodeEmitter() : Instructions(), CurrentLabel(0U) {}

const Instruction *CodeEmitter::Emit(frontEnd::TokenType Type,
                                     const Instruction::AnyOperand &Left,
                                     const Instruction::AnyOperand &Right) {
  Instructions.emplace_back(Instruction(CurrentLabel, Type, Left, Right));
  ++CurrentLabel;
  Instruction *I = &std::get<Instruction>(Instructions.back());
  return I;
}

const Instruction *CodeEmitter::Emit(const Instruction &Copy) {
  Instructions.push_back(Copy);
  ++CurrentLabel;
  Instruction *I = &std::get<Instruction>(Instructions.back());
  return I;
}

const UnaryInstruction *
CodeEmitter::Emit(const UnaryInstruction::AnyOperand &Op) {
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

const Jump *CodeEmitter::EmitJump(unsigned ToLabel) {
  Instructions.emplace_back(Jump(ToLabel));
  Jump *I = &std::get<Jump>(Instructions.back());
  return I;
}

void CodeEmitter::Dump() {
  for (const AnyInstruction &Any : Instructions) {
    // clang-format off
    std::visit(Overload {
      [](const Instruction      &I) { std::cout << I.Dump() << std::endl; },
      [](const UnaryInstruction &I) { std::cout << I.Dump() << std::endl; },
      [](const IfInstruction    &I) { std::cout << I.Dump() << std::endl; },
      [](const GotoLabel        &I) { std::cout << I.Dump() << std::endl; },
      [](const Jump             &I) { std::cout << I.Dump() << std::endl; }
    }, Any);
    // clang-format on
  }
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

const std::list<AnyInstruction> &CodeEmitter::GetInstructions() const {
  return Instructions;
}

} // namespace middleEnd
} // namespace weak