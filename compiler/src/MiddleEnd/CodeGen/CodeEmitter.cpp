/* CodeEmitter.cpp - Helper class used to form IR.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/CodeGen/CodeEmitter.hpp"
#include "Utility/VariantOverload.hpp"
#include <iostream>

using namespace weak::middleEnd;

static void DumpImpl(const std::list<FunctionBlock> &Decls) {
  for (const auto &Decl : Decls) {
    std::cerr << "defun " << Decl.GetName() << std::endl;
    for (const auto &Instr : Decl.GetBody()) {
      std::visit(
          [](const auto &I) { std::cerr << "    " << I.Dump() << std::endl; },
          Instr);
    }
  }
}

namespace weak {
namespace middleEnd {

CodeEmitter::CodeEmitter() : FunctionInstructions(), CurrentLabel(0U) {}

void CodeEmitter::BeginFunction(std::string_view Name,
                                std::list<frontEnd::TokenType> &&Arguments) {
  FunctionArguments = std::move(Arguments);
  FunctionInstructions.clear();
  CurrentFunctionName = Name;
}

void CodeEmitter::EndFunction() {
  FunctionBlock Block;
  Block.SetName(std::move(CurrentFunctionName));
  Block.SetArguments(FunctionArguments.begin(), FunctionArguments.end());
  Block.SetBody(FunctionInstructions.begin(), FunctionInstructions.end());
  Functions.push_back(std::move(Block));
}

Instruction *CodeEmitter::Emit(frontEnd::TokenType Type,
                               const Instruction::AnyOperand &LHS,
                               const Instruction::AnyOperand &RHS) {
  Instruction *I = &std::get<Instruction>(FunctionInstructions.emplace_back(
      Instruction(CurrentLabel, Type, LHS, RHS)));
  ++CurrentLabel;
  return I;
}

Instruction *CodeEmitter::Emit(const Instruction &Copy) {
  ++CurrentLabel;
  return &std::get<Instruction>(FunctionInstructions.emplace_back(Copy));
}

UnaryInstruction *CodeEmitter::Emit(const UnaryInstruction::AnyOperand &Op) {
  UnaryInstruction *I = &std::get<UnaryInstruction>(
      FunctionInstructions.emplace_back(UnaryInstruction(CurrentLabel, Op)));
  ++CurrentLabel;
  return I;
}

IfInstruction *CodeEmitter::EmitIf(frontEnd::TokenType Operation,
                                   const Instruction::AnyOperand &Left,
                                   const Instruction::AnyOperand &Right,
                                   unsigned GotoLabel) {
  FunctionInstructions.emplace_back(
      IfInstruction(Operation, Left, Right, GotoLabel));
  IfInstruction *I = &std::get<IfInstruction>(FunctionInstructions.back());
  return I;
}

IfInstruction *CodeEmitter::EmitIf(const Instruction &Copy,
                                   unsigned GotoLabel) {
  // clang-format off
  std::visit(Overload{
    [&](const auto& LHS, const auto& RHS) {
      FunctionInstructions.emplace_back(
        IfInstruction(
          Copy.GetOp(), LHS, RHS, GotoLabel
        )
      );
    }
  }, Copy.GetLeftInstruction(), Copy.GetRightInstruction());
  // clang-format on
  IfInstruction *I = &std::get<IfInstruction>(FunctionInstructions.back());
  return I;
}

const GotoLabel *CodeEmitter::EmitGotoLabel(unsigned Label) {
  return &std::get<GotoLabel>(
      FunctionInstructions.emplace_back(GotoLabel(Label)));
}

Jump *CodeEmitter::EmitJump(unsigned Label) {
  return &std::get<Jump>(FunctionInstructions.emplace_back(Jump(Label)));
}

Call* CodeEmitter::EmitCall(std::string Name, std::list<Reference> &&Arguments) {
  return &std::get<Call>(FunctionInstructions.emplace_back(Call(std::move(Name), std::move(Arguments))));
}

void CodeEmitter::Dump() { DumpImpl(Functions); }

void CodeEmitter::Dump(const std::list<FunctionBlock> &Instructions) {
  DumpImpl(Instructions);
}

void CodeEmitter::RemoveLast() {
  // clang-format off
  std::visit(Overload{
    [this](const Instruction      &) { --CurrentLabel;   },
    [this](const UnaryInstruction &) { --CurrentLabel;   },
    [    ](const IfInstruction    &) { /* Do nothing. */ },
    [    ](const GotoLabel        &) { /* Do nothing. */ },
    [    ](const Jump             &) { /* Do nothing. */ },
    [    ](const Call             &) { /* Do nothing. */ }
  }, FunctionInstructions.back());
  // clang-format on
  FunctionInstructions.pop_back();
}

const AnyInstruction &CodeEmitter::GetLast() const {
  return FunctionInstructions.back();
}

const std::list<AnyInstruction> &CodeEmitter::GetFunctionInstructions() const {
  return FunctionInstructions;
}

const std::list<FunctionBlock> &CodeEmitter::GetFunctions() const {
  return Functions;
}

} // namespace middleEnd
} // namespace weak