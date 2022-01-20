/* CodeGen.cpp - Intermediate code generator.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/CodeGen/CodeGen.hpp"
#include "FrontEnd/AST/ASTBinaryOperator.hpp"
#include "FrontEnd/AST/ASTBooleanLiteral.hpp"
#include "FrontEnd/AST/ASTBreakStmt.hpp"
#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include "FrontEnd/AST/ASTContinueStmt.hpp"
#include "FrontEnd/AST/ASTDoWhileStmt.hpp"
#include "FrontEnd/AST/ASTFloatingPointLiteral.hpp"
#include "FrontEnd/AST/ASTForStmt.hpp"
#include "FrontEnd/AST/ASTFunctionCall.hpp"
#include "FrontEnd/AST/ASTFunctionDecl.hpp"
#include "FrontEnd/AST/ASTIfStmt.hpp"
#include "FrontEnd/AST/ASTIntegerLiteral.hpp"
#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/AST/ASTReturnStmt.hpp"
#include "FrontEnd/AST/ASTStringLiteral.hpp"
#include "FrontEnd/AST/ASTSymbol.hpp"
#include "FrontEnd/AST/ASTUnaryOperator.hpp"
#include "FrontEnd/AST/ASTVarDecl.hpp"
#include "FrontEnd/AST/ASTWhileStmt.hpp"
#include "Utility/Diagnostic.hpp"
#include "Utility/VariantOverload.hpp"

using namespace weak::frontEnd;

namespace weak {
namespace middleEnd {

CodeGen::CodeGen(Storage *TheVariablePool, ASTNode *TheRootNode)
    : RootNode(TheRootNode), Emitter(), LastInstruction(0),
      CurrentGotoLabel(0U), VariablePool(TheVariablePool) {}

template <typename DataType>
IfInstruction *
CodeGen::CreateConditionalInstruction(const ASTNode *Node) const {
  auto *Value = static_cast<const DataType *>(Node);
  IfInstruction *If =
      Emitter.EmitIf(TokenType::NEQ, Value->GetValue(), 0, /*GotoLabel=*/0U);
  return If;
}

template IfInstruction *
CodeGen::CreateConditionalInstruction<ASTIntegerLiteral>(const ASTNode *) const;
template IfInstruction *
CodeGen::CreateConditionalInstruction<ASTBooleanLiteral>(const ASTNode *) const;
template IfInstruction *
CodeGen::CreateConditionalInstruction<ASTFloatingPointLiteral>(
    const ASTNode *) const;

IfInstruction *CodeGen::EmitCondition(const ASTNode *Condition) const {
  switch (Condition->GetASTType()) {
  case ASTType::INTEGER_LITERAL: {
    return CreateConditionalInstruction<ASTIntegerLiteral>(Condition);
    break;
  }
  case ASTType::BOOLEAN_LITERAL: {
    return CreateConditionalInstruction<ASTBooleanLiteral>(Condition);
    break;
  }
  case ASTType::FLOATING_POINT_LITERAL: {
    return CreateConditionalInstruction<ASTFloatingPointLiteral>(Condition);
    break;
  }
  case ASTType::BINARY: {
    Condition->Accept(this);
    Instruction *Instr = &std::get<Instruction>(LastInstruction);
    Emitter.RemoveLast();
    return Emitter.EmitIf(*Instr, /*GotoLabel=*/0U);
  }
  default:
    DiagnosticError() << "Should not reach here.";
    UnreachablePoint();
  }
}

const std::list<AnyInstruction> &CodeGen::CreateCode() {
  RootNode->Accept(this);
  return Emitter.GetInstructions();
}

void CodeGen::Visit(const frontEnd::ASTCompoundStmt *Compound) const {
  VariablePool->ScopeBegin();

  for (const auto &Stmt : Compound->GetStmts())
    Stmt->Accept(this);

  VariablePool->ScopeEnd();
}

void CodeGen::Visit(const frontEnd::ASTFunctionDecl *FunctionDecl) const {
  FunctionDecl->GetBody()->Accept(this);
}

void CodeGen::EmitAssignment(const frontEnd::ASTBinaryOperator *Binary) const {
  Binary->GetLHS()->Accept(this);
  InstructionReference Ref = std::get<InstructionReference>(LastInstruction);
  Binary->GetRHS()->Accept(this);

  // clang-format off
  std::visit(Overload {
    /// Instruction is already emitted, so we should to remove it
    /// and emit again to be able to change it's label.
    [this, &Ref](const Instruction &I) {
      Emitter.RemoveLast();
      auto *Instr = Emitter.Emit(I);
      Instr->SetLabelNo(Ref.GetLabelNo());
      LastInstruction = *Instr;
    },
    /// In all other cases unary instruction should be produced.
    [this, &Ref](const auto &I) {
      UnaryInstruction *Unary = Emitter.Emit(I);
      Unary->SetLabelNo(Ref.GetLabelNo());
      LastInstruction = *Unary;
    }
  }, LastInstruction);
  // clang-format on
}

void CodeGen::Visit(const frontEnd::ASTBinaryOperator *Binary) const {
  if (Binary->GetOperation() == TokenType::ASSIGN) {
    EmitAssignment(Binary);
    return;
  }

  Binary->GetLHS()->Accept(this);
  auto LHS = LastInstruction;

  Binary->GetRHS()->Accept(this);
  auto RHS = LastInstruction;

  TokenType Op = Binary->GetOperation();
  using Ref = InstructionReference;
  // clang-format off
  std::visit(Overload {
    [this, Op](const Instruction& L, const Instruction& R) {
      Emitter.Emit(Op, Ref(L), Ref(R));
    },
    [this, Op](const Instruction& L, const auto &R) {
      Emitter.Emit(Op, Ref(L), R);
    },
    [this, Op](const auto &L, const Instruction& R) {
      Emitter.Emit(Op, L, Ref(R));
    },
    [this, Op](const auto &L, const auto &R) {
      Emitter.Emit(Op, L, R);
    }
  }, LHS, RHS);
  // clang-format on

  LastInstruction = std::get<Instruction>(Emitter.GetInstructions().back());
}

void CodeGen::Visit(const frontEnd::ASTIntegerLiteral *Integer) const {
  LastInstruction = Integer->GetValue();
}

void CodeGen::Visit(const frontEnd::ASTVarDecl *VarDecl) const {
  VarDecl->GetDeclareBody()->Accept(this);

  unsigned Label = 0U;

  // clang-format off
  std::visit(Overload {
    [this  ](signed I) { LastInstruction = InstructionReference(*Emitter.Emit(I)); },
    [this  ](double I) { LastInstruction = I; },
    [this  ](bool   I) { LastInstruction = I; },
    [&Label](const UnaryInstruction     &I) { Label = I.GetLabelNo(); },
    [&Label](const Instruction          &I) { Label = I.GetLabelNo(); },
    [      ](const InstructionReference & ) { /* Do nothing. */       },
  }, LastInstruction);
  // clang-format on

  auto *Record = VariablePool->GetByName(VarDecl->GetSymbolName());
  Record->DataType = VarDecl->GetDataType();
  Record->TemporaryLabel = Label;

  std::visit([&Record](const auto &I) { Record->StoredValue = I; },
             LastInstruction);
}

void CodeGen::Visit(const frontEnd::ASTBooleanLiteral *Boolean) const {
  LastInstruction = Boolean->GetValue();
}

void CodeGen::Visit(const frontEnd::ASTBreakStmt *) const {}
void CodeGen::Visit(const frontEnd::ASTContinueStmt *) const {}

/*!               do { body } while (cond); after
 *
 *  LOOP:         instr1
 *                instr2
 *                if cond then goto LOOP
 *                goto EXIT
 *  EXIT:         after instr
 */
void CodeGen::Visit(const frontEnd::ASTDoWhileStmt *DoWhile) const {
  unsigned SavedGotoLabel = CurrentGotoLabel++;
  Emitter.EmitGotoLabel(SavedGotoLabel);

  DoWhile->GetBody()->Accept(this);

  IfInstruction *Condition = EmitCondition(DoWhile->GetCondition().get());

  Emitter.EmitJump(SavedGotoLabel);
  Emitter.EmitGotoLabel(CurrentGotoLabel);
  Condition->SetGotoLabel(CurrentGotoLabel);
  CurrentGotoLabel++;
}

void CodeGen::Visit(const frontEnd::ASTFloatingPointLiteral *Float) const {
  LastInstruction = Float->GetValue();
}

/*!               for (init; cond; inc) { body }
 *
 *                init instr
 *  COND:         if !cond goto EXIT
 *                body instr 1
 *                body instr 2
 *                inc instr
 *                goto COND
 *  EXIT:         after instr
 */
/// \todo Clean implementation.
void CodeGen::Visit(const frontEnd::ASTForStmt *For) const {
  For->GetInit()->Accept(this);

  unsigned ForStartLabel = CurrentGotoLabel++;
  Emitter.EmitGotoLabel(ForStartLabel);

  IfInstruction *Condition = EmitCondition(For->GetCondition().get());

  Jump *FailureFallback = Emitter.EmitJump(CurrentGotoLabel);

  Emitter.EmitGotoLabel(CurrentGotoLabel);
  unsigned Saved = CurrentGotoLabel++;
  For->GetBody()->Accept(this);
  For->GetIncrement()->Accept(this);
  FailureFallback->SetLabelNo(CurrentGotoLabel);
  Condition->SetGotoLabel(ForStartLabel + 1);
  Emitter.EmitJump(Saved - 1);
  Emitter.EmitGotoLabel(CurrentGotoLabel);
  ++CurrentGotoLabel;
}

void CodeGen::Visit(const frontEnd::ASTFunctionCall *) const {}

/*!               if !cond then goto EXIT
 *                instr1
 *                instr2
 *  EXIT:         after if instr
 *
 *                if !cond then goto ELSE
 *                instr1
 *                instr2
 *                goto EXIT
 *  ELSE:         else instr1
 *                else instr2
 *  EXIT:         after instr
 */
void CodeGen::Visit(const frontEnd::ASTIfStmt *If) const {
  unsigned SavedGotoLabel = CurrentGotoLabel++;

  IfInstruction *Condition = EmitCondition(If->GetCondition().get());

  Jump *FailureFallback = Emitter.EmitJump(0);
  Emitter.EmitGotoLabel(SavedGotoLabel);
  Condition->SetGotoLabel(SavedGotoLabel);
  If->GetThenBody()->Accept(this);
  Emitter.EmitGotoLabel(CurrentGotoLabel);
  FailureFallback->SetLabelNo(CurrentGotoLabel);
  ++CurrentGotoLabel;
  if (If->GetElseBody())
    If->GetElseBody()->Accept(this);
}

void CodeGen::Visit(const frontEnd::ASTReturnStmt *) const {}
void CodeGen::Visit(const frontEnd::ASTStringLiteral *) const {}

void CodeGen::Visit(const frontEnd::ASTSymbol *Symbol) const {
  auto *Record = VariablePool->GetByName(Symbol->GetValue());

  /// If variable type was not set, we have not the complete definition.
  if (Record->DataType == TokenType::NONE) {
    DiagnosticError(Symbol->GetLineNo(), Symbol->GetColumnNo())
        << "Variable not found: " << Symbol->GetValue();
    UnreachablePoint();
  }

  std::visit([this](auto I) { LastInstruction = I; }, Record->StoredValue);
}

void CodeGen::Visit(const frontEnd::ASTUnaryOperator *Unary) const {
  Unary->GetOperand()->Accept(this);

  std::function<void(TokenType)> VisitUnary;

  // clang-format off
  std::visit(Overload {
    [](const Instruction      &) {},
    [](const UnaryInstruction &) {},
    [this, &VisitUnary](const InstructionReference &I) {
      VisitUnary = [this, &I](TokenType Operation) {
        auto *New = Emitter.Emit(Operation, I, 1);
        New->SetLabelNo(I.GetLabelNo());
        LastInstruction = *New;
      };
    },
    [this, &VisitUnary](auto I) {
      VisitUnary = [this, &I](TokenType Operation) {
        LastInstruction = *Emitter.Emit(Operation, I, 1);
      };
    }
  }, LastInstruction);
  // clang-format on

  switch (Unary->GetOperation()) {
  case TokenType::INC:
    VisitUnary(TokenType::PLUS);
    break;
  case TokenType::DEC:
    VisitUnary(TokenType::MINUS);
    break;
  default:
    break;
  }
}

/*  while (cond) { body() } after_body()
 *
 *  COND:           if cond goto BODY
 *                  goto EXIT
 *  BODY:           body instr1
 *                  body instr2
 *                  goto COND
 *  EXIT:           after instr
 */
void CodeGen::Visit(const frontEnd::ASTWhileStmt *While) const {
  if (!std::holds_alternative<GotoLabel>(Emitter.GetInstructions().back())) {
    Emitter.EmitGotoLabel(CurrentGotoLabel);
    ++CurrentGotoLabel;
  }

  IfInstruction *Condition = EmitCondition(While->GetCondition().get());
  Condition->SetGotoLabel(CurrentGotoLabel);
  auto *Fallback = Emitter.EmitJump(/*GotoLabel=*/0);
  Emitter.EmitGotoLabel(CurrentGotoLabel);
  unsigned Saved = CurrentGotoLabel++;
  While->GetBody()->Accept(this);
  Emitter.EmitJump(Saved - 1);
  Emitter.EmitGotoLabel(CurrentGotoLabel);
  Fallback->SetLabelNo(CurrentGotoLabel);
  ++CurrentGotoLabel;
}

} // namespace middleEnd
} // namespace weak