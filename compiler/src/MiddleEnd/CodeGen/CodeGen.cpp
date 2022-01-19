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
IfInstruction *CodeGen::CreateConditionalInstruction(ASTNode *Node) const {
  auto *Value = static_cast<DataType *>(Node);
  IfInstruction *If =
      Emitter.EmitIf(TokenType::NEQ, Value->GetValue(), 0, /*GotoLabel=*/0);
  return If;
}

template IfInstruction *
CodeGen::CreateConditionalInstruction<ASTIntegerLiteral>(ASTNode *) const;
template IfInstruction *
CodeGen::CreateConditionalInstruction<ASTBooleanLiteral>(ASTNode *) const;
template IfInstruction *
CodeGen::CreateConditionalInstruction<ASTFloatingPointLiteral>(ASTNode *) const;

void CodeGen::CreateCode() {
  RootNode->Accept(this);
  Emitter.Dump();
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

void CodeGen::Visit(const frontEnd::ASTBinaryOperator *Binary) const {
  //  if (Binary->GetOperation() == TokenType::ASSIGN) {
  //    using Ref = InstructionReference;
  //    ASTSymbol *Symbol = static_cast<ASTSymbol *>(Binary->GetLHS().get());
  //    Storage::Record *Record = VariablePool->GetByName(Symbol->GetValue());
  //
  //    Binary->GetRHS()->Accept(this);
  //
  //    // clang-format off
  //    std::visit(Overload {
  //      [&Record](const UnaryInstruction &I) {
  //        Record->StoredValue = I;
  //      },
  //      [&Record](const Instruction &I) {
  //        Record->StoredValue = I;
  //      },
  //      [&Record](auto I) {
  //        Record->StoredValue = I;
  //      }
  //    }, LastInstruction);
  //    // clang-format on
  //
  //    // clang-format off
  //    std::visit(Overload {
  //      [this](const UnaryInstruction &I) {
  //        LastInstruction = *Emitter.Emit(Ref(I));
  //      },
  //      [this](const Instruction &I) {
  //        LastInstruction = *Emitter.Emit(Ref(I));
  //      },
  //      [this, &Record](auto I) {
  //        UnaryInstruction *U = Emitter.Emit(I);
  //        U->SetLabelNo(Record->TemporaryLabel);
  //        LastInstruction = *U;
  //      }
  //    }, Record->StoredValue);
  //    // clang-format on
  //
  //    return;
  //  }

  Binary->GetLHS()->Accept(this);
  auto LHS = LastInstruction;

  Binary->GetRHS()->Accept(this);
  auto RHS = LastInstruction;

  using Ref = InstructionReference;
  // clang-format off
  std::visit(Overload {
    [this, &Binary](const Instruction& L, const Instruction& R) {
      Emitter.Emit(Binary->GetOperation(), Ref(L), Ref(R));
    },
    [this, &Binary](const Instruction& L, auto R) {
      Emitter.Emit(Binary->GetOperation(), Ref(L), R);
    },
    [this, &Binary](auto L, const Instruction& R) {
      Emitter.Emit(Binary->GetOperation(), L, Ref(R));
    },
    [this, &Binary](auto L, auto R) {
      Emitter.Emit(Binary->GetOperation(), L, R);
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

  unsigned Label = 0;

  // clang-format off
  std::visit(Overload {
    [      &Label](const UnaryInstruction &I) { Label = I.GetLabelNo(); /* Do nothing. */ },
    [      &Label](const Instruction      &I) { Label = I.GetLabelNo(); /* Do nothing. */ },
    [this, &Label](signed I) {
      LastInstruction = *Emitter.Emit(I);
      Label = std::get<UnaryInstruction>(LastInstruction).GetLabelNo();
    },
    [this, &Label](double I) {
      LastInstruction = *Emitter.Emit(I);
      Label = std::get<UnaryInstruction>(LastInstruction).GetLabelNo();
    },
    [this, &Label](bool I) {
      LastInstruction = *Emitter.Emit(I);
      Label = std::get<UnaryInstruction>(LastInstruction).GetLabelNo();
    }
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
 *  EXIT:         after instr1
 *                after instr2
 */
void CodeGen::Visit(const frontEnd::ASTDoWhileStmt *DoWhile) const {
  unsigned SavedGotoLabel = CurrentGotoLabel++;
  Emitter.EmitGotoLabel(SavedGotoLabel);

  DoWhile->GetBody()->Accept(this);

  IfInstruction *ConditionInstruction = nullptr;

  switch (auto *Condition = DoWhile->GetCondition().get();
          Condition->GetASTType()) {
  case ASTType::INTEGER_LITERAL: {
    ConditionInstruction =
        CreateConditionalInstruction<ASTIntegerLiteral>(Condition);
    break;
  }
  case ASTType::BOOLEAN_LITERAL: {
    ConditionInstruction =
        CreateConditionalInstruction<ASTBooleanLiteral>(Condition);
    break;
  }
  case ASTType::FLOATING_POINT_LITERAL: {
    ConditionInstruction =
        CreateConditionalInstruction<ASTFloatingPointLiteral>(Condition);
    break;
  }
  case ASTType::BINARY: {
    DoWhile->GetCondition()->Accept(this);
    Instruction *Instr = &std::get<Instruction>(LastInstruction);
    Emitter.RemoveLast();
    ConditionInstruction = Emitter.EmitIf(*Instr, /*GotoLabel=*/0);
    break;
  }
  default:
    break;
  }

  Emitter.EmitJump(SavedGotoLabel);

  Emitter.EmitGotoLabel(CurrentGotoLabel);

  ConditionInstruction->SetGotoLabel(CurrentGotoLabel);
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
 *  EXIT:         after for instr
 */
/// \todo Clean implementation.
void CodeGen::Visit(const frontEnd::ASTForStmt *For) const {
  For->GetInit()->Accept(this);

  unsigned SavedGotoLabel = CurrentGotoLabel++;
  Emitter.EmitGotoLabel(SavedGotoLabel);

  IfInstruction *ConditionalInstruction = nullptr;

  switch (auto *Condition = For->GetCondition().get();
          Condition->GetASTType()) {
  case ASTType::INTEGER_LITERAL: {
    ConditionalInstruction =
        CreateConditionalInstruction<ASTIntegerLiteral>(Condition);
    break;
  }
  case ASTType::BOOLEAN_LITERAL: {
    ConditionalInstruction =
        CreateConditionalInstruction<ASTIntegerLiteral>(Condition);
    break;
  }
  case ASTType::FLOATING_POINT_LITERAL: {
    ConditionalInstruction =
        CreateConditionalInstruction<ASTIntegerLiteral>(Condition);
    break;
  }
  case ASTType::BINARY: {
    For->GetCondition()->Accept(this);
    Instruction *Instr = &std::get<Instruction>(LastInstruction);
    Emitter.RemoveLast();
    ConditionalInstruction = Emitter.EmitIf(*Instr, /*GotoLabel=*/0);
    break;
  }
  default:
    break;
  }

  auto *FailureFallback = Emitter.EmitJump(CurrentGotoLabel);

  unsigned FallbackLabel = CurrentGotoLabel++;

  Emitter.EmitGotoLabel(FallbackLabel);

  For->GetBody()->Accept(this);
  For->GetIncrement()->Accept(this);

  Emitter.EmitJump(FallbackLabel - 1);

  ++SavedGotoLabel;
  ConditionalInstruction->SetGotoLabel(SavedGotoLabel);
  Emitter.EmitGotoLabel(CurrentGotoLabel);
  FailureFallback->SetLabelNo(CurrentGotoLabel);
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
 *  EXIT:         after if instr
 */

void CodeGen::Visit(const frontEnd::ASTIfStmt *If) const {
  unsigned SavedGotoLabel = CurrentGotoLabel++;

  IfInstruction *ConditionalInstruction = nullptr;

  switch (auto *Condition = If->GetCondition().get(); Condition->GetASTType()) {
  case ASTType::INTEGER_LITERAL: {
    ConditionalInstruction =
        CreateConditionalInstruction<ASTIntegerLiteral>(Condition);
    break;
  }
  case ASTType::BOOLEAN_LITERAL: {
    ConditionalInstruction =
        CreateConditionalInstruction<ASTBooleanLiteral>(Condition);
    break;
  }
  case ASTType::FLOATING_POINT_LITERAL: {
    ConditionalInstruction =
        CreateConditionalInstruction<ASTFloatingPointLiteral>(Condition);
    break;
  }
  case ASTType::BINARY: {
    If->GetCondition()->Accept(this);

    Emitter.RemoveLast();
    ConditionalInstruction =
        Emitter.EmitIf(std::get<Instruction>(LastInstruction), /*GotoLabel=*/0);
    break;
  }
  default:
    break;
  }

  Jump *FailureFallback = Emitter.EmitJump(0);

  Emitter.EmitGotoLabel(SavedGotoLabel);
  ConditionalInstruction->SetGotoLabel(SavedGotoLabel);

  If->GetThenBody()->Accept(this);

  Emitter.EmitGotoLabel(CurrentGotoLabel);
  FailureFallback->SetLabelNo(CurrentGotoLabel);
  ++CurrentGotoLabel;

  if (If->GetElseBody()) {
    If->GetElseBody()->Accept(this);
  }
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
  using Ref = InstructionReference;

  auto VisitUnary = [this](TokenType Operation, auto &&Instr) {
    // clang-format off
    std::visit(Overload {
      [this, Op = Operation](const Instruction &I) {
        Emitter.Emit(Op, Ref(I), 1);
      },
      [this, Op = Operation](auto I) {
        Emitter.Emit(Op, I, 1);
      }
    }, Instr);
    // clang-format on
  };

  switch (Unary->GetOperation()) {
  case TokenType::INC:
    VisitUnary(TokenType::PLUS, LastInstruction);
    break;
  case TokenType::DEC:
    VisitUnary(TokenType::MINUS, LastInstruction);
    break;
  default:
    break;
  }

  LastInstruction = std::get<Instruction>(Emitter.GetInstructions().back());
}

/*  while (cond) { body() } after_body()
 *
 *  COND:           if cond goto BODY
 *                  goto EXIT
 *  BODY:           body instr1
 *                  body instr2
 *                  goto COND
 *  EXIT:           after while instr
 */
void CodeGen::Visit(const frontEnd::ASTWhileStmt *While) const {
  if (!std::holds_alternative<GotoLabel>(Emitter.GetInstructions().back())) {
    Emitter.EmitGotoLabel(CurrentGotoLabel);
    ++CurrentGotoLabel;
  }

  IfInstruction *ConditionalInstruction = nullptr;

  switch (auto *Condition = While->GetCondition().get();
          Condition->GetASTType()) {
  case ASTType::INTEGER_LITERAL: {
    ConditionalInstruction =
        CreateConditionalInstruction<ASTIntegerLiteral>(Condition);
    break;
  }
  case ASTType::BOOLEAN_LITERAL: {
    ConditionalInstruction =
        CreateConditionalInstruction<ASTBooleanLiteral>(Condition);
    break;
  }
  case ASTType::FLOATING_POINT_LITERAL: {
    ConditionalInstruction =
        CreateConditionalInstruction<ASTFloatingPointLiteral>(Condition);
    break;
  }
  case ASTType::BINARY: {
    While->GetCondition()->Accept(this);
    Instruction *Instr = &std::get<Instruction>(LastInstruction);
    Emitter.RemoveLast();
    ConditionalInstruction = Emitter.EmitIf(*Instr, /*GotoLabel=*/0);
    break;
  }
  default:
    break;
  }

  unsigned Saved = CurrentGotoLabel++;

  ConditionalInstruction->SetGotoLabel(Saved);

  auto *Fallback = Emitter.EmitJump(/*GotoLabel=*/0);

  Emitter.EmitGotoLabel(Saved);

  While->GetBody()->Accept(this);

  Emitter.EmitJump(Saved - 1);

  Emitter.EmitGotoLabel(CurrentGotoLabel);
  Fallback->SetLabelNo(CurrentGotoLabel);

  ++CurrentGotoLabel;
}

} // namespace middleEnd
} // namespace weak