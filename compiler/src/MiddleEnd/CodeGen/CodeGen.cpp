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
#include <functional>

using namespace weak::frontEnd;

namespace weak {
namespace middleEnd {

CodeGen::CodeGen(Storage *TheVariablePool, ASTNode *TheRootNode)
    : RootNode(TheRootNode), Emitter(), LastInstruction(0),
      CurrentGotoLabel(0U), VariablePool(TheVariablePool), LoopHasBreak(false),
      LoopHasContinue(false) {}

template <typename DataType>
IfInstruction *CodeGen::CreateUnaryCondition(const ASTNode *Node) const {
  auto *Value = static_cast<const DataType *>(Node);
  IfInstruction *If =
      Emitter.EmitIf(TokenType::NEQ, Value->GetValue(), 0, /*GotoLabel=*/0U);
  return If;
}

template IfInstruction *
CodeGen::CreateUnaryCondition<ASTIntegerLiteral>(const ASTNode *) const;
template IfInstruction *
CodeGen::CreateUnaryCondition<ASTBooleanLiteral>(const ASTNode *) const;
template IfInstruction *
CodeGen::CreateUnaryCondition<ASTFloatingPointLiteral>(const ASTNode *) const;

IfInstruction *CodeGen::EmitCondition(const ASTNode *Condition) const {
  switch (Condition->GetASTType()) {
  case ASTType::INTEGER_LITERAL:
    return CreateUnaryCondition<ASTIntegerLiteral>(Condition);

  case ASTType::BOOLEAN_LITERAL:
    return CreateUnaryCondition<ASTBooleanLiteral>(Condition);

  case ASTType::FLOATING_POINT_LITERAL:
    return CreateUnaryCondition<ASTFloatingPointLiteral>(Condition);

  case ASTType::BINARY: {
    Condition->Accept(this);
    Instruction *Instr = &std::get<Instruction>(LastInstruction);
    Emitter.RemoveLast();
    return Emitter.EmitIf(*Instr, /*GotoLabel=*/0U);
  }
  default:
    CompileError() << "Should not reach here.";
    UnreachablePoint();
  }
}

void CodeGen::EmitCondition(const frontEnd::ASTNode *Node,
                            unsigned Label) const {
  IfInstruction *If = EmitCondition(Node);
  If->SetGotoLabel(Label);
}

void CodeGen::EmitLoopBody(Jump *&BreakJump, Jump *&ContinueJump,
                           const ASTCompoundStmt *Body) const {
  VariablePool->ScopeBegin();

  for (const auto &Stmt : Body->GetStmts()) {
    Stmt->Accept(this);
    if (LoopHasBreak) {
      BreakJump = Emitter.EmitJump(/*GotoLabel=*/0U);
      LoopHasBreak = false;
    }
    if (LoopHasContinue) {
      ContinueJump = Emitter.EmitJump(/*GotoLabel=*/0U);
      LoopHasContinue = false;
    }
  }

  VariablePool->ScopeEnd();
}

const std::list<FunctionBlock> &CodeGen::CreateCode() {
  RootNode->Accept(this);
  return Emitter.GetFunctions();
}

void CodeGen::Visit(const ASTCompoundStmt *Compound) const {
  VariablePool->ScopeBegin();

  for (const auto &Stmt : Compound->GetStmts())
    Stmt->Accept(this);

  LoopHasBreak = false;
  LoopHasContinue = false;

  VariablePool->ScopeEnd();
}

void CodeGen::Visit(const ASTFunctionDecl *FunctionDecl) const {
  std::list<TokenType> Arguments;
  for (const auto &Argument : FunctionDecl->GetArguments())
    Arguments.push_back(
        static_cast<ASTVarDecl *>(Argument.get())->GetDataType());

  Emitter.BeginFunction(FunctionDecl->GetName(), std::move(Arguments));
  FunctionDecl->GetBody()->Accept(this);
  Emitter.EndFunction();
}

void CodeGen::EmitAssignment(const ASTBinaryOperator *Binary) const {
  Binary->GetLHS()->Accept(this);
  Reference Symbol = std::get<Reference>(LastInstruction);
  Binary->GetRHS()->Accept(this);

  if (std::get_if<Instruction>(&LastInstruction))
    Emitter.RemoveLast();

  // clang-format off
  std::visit([&](const auto &Value) {
    Emitter.Emit(Value)->SetLabelNo(Symbol.GetLabelNo());
  }, LastInstruction);
  // clang-format on
}

void CodeGen::Visit(const ASTBinaryOperator *Binary) const {
  if (Binary->GetOperation() == TokenType::ASSIGN) {
    EmitAssignment(Binary);
    return;
  }

  Binary->GetLHS()->Accept(this);
  auto LHS = LastInstruction;

  Binary->GetRHS()->Accept(this);
  auto RHS = LastInstruction;

  TokenType Op = Binary->GetOperation();
  // clang-format off
  std::visit(Overload {
    [&](const Instruction& L, const Instruction& R) {
      Emitter.Emit(Op, Reference(L), Reference(R));
    },
    [&](const Instruction& L, const auto &R) {
      Emitter.Emit(Op, Reference(L), R);
    },
    [&](const auto &L, const Instruction& R) {
      Emitter.Emit(Op, L, Reference(R));
    },
    [&](const auto &L, const auto &R) {
      Emitter.Emit(Op, L, R);
    }
  }, LHS, RHS);
  // clang-format on

  LastInstruction = std::get<Instruction>(Emitter.GetLast());
}

void CodeGen::Visit(const ASTIntegerLiteral *Integer) const {
  LastInstruction = Integer->GetValue();
}

void CodeGen::Visit(const ASTVarDecl *VarDecl) const {
  VarDecl->GetDeclareBody()->Accept(this);

  unsigned Label = 0U;

  // clang-format off
  std::visit(Overload {
    [&](signed                  I) { LastInstruction = Reference(*Emitter.Emit(I)); },
    [&](double                  I) { LastInstruction = Reference(*Emitter.Emit(I)); },
    [&](bool                    I) { LastInstruction = Reference(*Emitter.Emit(I)); },
    [&](const UnaryInstruction &I) { LastInstruction = Reference(I); Label = I.GetLabelNo(); },
    [&](const Instruction      &I) { LastInstruction = Reference(I); Label = I.GetLabelNo(); },
    [ ](const Reference        & ) { /* Do nothing. */ },
  }, LastInstruction);
  // clang-format on

  auto *Record = VariablePool->GetByName(VarDecl->GetSymbolName());
  Record->DataType = VarDecl->GetDataType();
  Record->TemporaryLabel = Label;
  Record->VarReference = std::get<Reference>(LastInstruction);
}

void CodeGen::Visit(const ASTBooleanLiteral *Boolean) const {
  LastInstruction = Boolean->GetValue();
}

void CodeGen::Visit(const ASTBreakStmt *) const { LoopHasBreak = true; }

void CodeGen::Visit(const ASTContinueStmt *) const { LoopHasContinue = true; }

/*!               do { body } while (cond); after
 *
 *  LOOP:         instr1
 *                instr2
 *                if cond then goto LOOP
 *                goto EXIT
 *  EXIT:         after instr
 */
void CodeGen::Visit(const ASTDoWhileStmt *DoWhile) const {
  unsigned BeginLabel = CurrentGotoLabel++;
  Emitter.EmitGotoLabel(BeginLabel);
  Jump *BreakJump = nullptr;
  Jump *ContinueJump = nullptr;
  EmitLoopBody(BreakJump, ContinueJump, DoWhile->GetBody().get());
  /// If break exists, we want to jump outside the loop.
  if (BreakJump)
    BreakJump->SetLabelNo(CurrentGotoLabel);
  /// If continue exists, we want to immediate jump again to the condition.
  if (ContinueJump)
    ContinueJump->SetLabelNo(BeginLabel);
  /// If condition evaluates to true, we want to jump again to the condition.
  EmitCondition(DoWhile->GetCondition().get(), BeginLabel);
  /// Otherwise control flow should simply fall through to the next statement.
  ++CurrentGotoLabel;
  /// Emit additional label if we have break, to be able jump outside.
  if (BreakJump)
    Emitter.EmitGotoLabel(BreakJump->GetLabel());
}

void CodeGen::Visit(const ASTFloatingPointLiteral *Float) const {
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
void CodeGen::Visit(const frontEnd::ASTForStmt *For) const {
  For->GetInit()->Accept(this);
  /// Label that represents a loop body start.
  unsigned BeginLabel = CurrentGotoLabel;
  Emitter.EmitGotoLabel(CurrentGotoLabel);
  EmitCondition(For->GetCondition().get(), ++CurrentGotoLabel);
  /// This is used to jump to condition check again at the end of iteration.
  Jump *Fallback = Emitter.EmitJump(/*GotoLabel=*/0U);
  /// We will fall here if the condition was evaluated as true.
  Emitter.EmitGotoLabel(CurrentGotoLabel++);
  Jump *BreakJump = nullptr;
  Jump *ContinueJump = nullptr;
  EmitLoopBody(BreakJump, ContinueJump, For->GetBody().get());
  /// If break exists, we want to jump outside the loop.
  if (BreakJump)
    BreakJump->SetLabelNo(CurrentGotoLabel);
  /// If continue exists, we want to immediate jump again to the condition.
  if (ContinueJump)
    ContinueJump->SetLabelNo(BeginLabel);
  For->GetIncrement()->Accept(this);
  /// We want to jump at the exit of our loop if condition
  /// was failed.
  Fallback->SetLabelNo(CurrentGotoLabel);
  /// Jump again to condition.
  Emitter.EmitJump(BeginLabel);
  /// This a loop exit label.
  /// Example:
  ///     if 1 < 1 goto L1
  ///     goto L2
  /// L1: instr1
  ///     instr2
  /// L2: /* The end of loop. */
  Emitter.EmitGotoLabel(CurrentGotoLabel++);
}

void CodeGen::Visit(const ASTFunctionCall * /*Call*/) const {
  /// 1. Storage must store also functions (name, return type, types of
  /// arguments,
  ///    reference to function (???)).
  /// 2. We need to get function record in storage by name or attribute.
  /// 3. We need to compare types of function declaration and call
  ///    arguments.
  /// 4. Next we should to create temporary variables for arguments
  ///    (if any).
  /// 5. Finally, we should to emit function call instruction with
  ///    computed references as arguments.
}

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
void CodeGen::Visit(const ASTIfStmt *If) const {
  /// If condition evaluates to true, we jump to the body.
  EmitCondition(If->GetCondition().get(), CurrentGotoLabel);
  unsigned FallbackLabel = CurrentGotoLabel++;
  /// This is a label used to jump outside the if block on failure.
  Jump *Fallback = Emitter.EmitJump(/*GotoLabel=*/0U);
  /// We will fall here on successful condition.
  Emitter.EmitGotoLabel(FallbackLabel);
  If->GetThenBody()->Accept(this);
  /// Create exit label and setup the failure jump statement.
  Emitter.EmitGotoLabel(CurrentGotoLabel);
  Fallback->SetLabelNo(CurrentGotoLabel);
  ++CurrentGotoLabel;
  if (If->GetElseBody())
    If->GetElseBody()->Accept(this);
}

void CodeGen::Visit(const ASTReturnStmt *) const {}
void CodeGen::Visit(const ASTStringLiteral *) const {}

void CodeGen::Visit(const ASTSymbol *Symbol) const {
  auto *Record = VariablePool->GetByName(Symbol->GetValue());

  /// If variable type was not set, we have not the complete definition.
  if (Record->DataType == TokenType::NONE) {
    CompileError(Symbol->GetLineNo(), Symbol->GetColumnNo())
        << "Variable not found: " << Symbol->GetValue();
    UnreachablePoint();
  }

  LastInstruction = Record->VarReference;
}

void CodeGen::Visit(const ASTUnaryOperator *Unary) const {
  Unary->GetOperand()->Accept(this);

  std::function<void(TokenType)> VisitUnary;

  // clang-format off
  std::visit(Overload {
    [ ](const Instruction      &) {},
    [ ](const UnaryInstruction &) {},
    [&](const Reference &I) {
      VisitUnary = [&](TokenType Operation) {
        auto *New = Emitter.Emit(Operation, I, 1);
        New->SetLabelNo(I.GetLabelNo());
        LastInstruction = *New;
      };
    },
    [&](auto I) {
      VisitUnary = [&](TokenType Operation) {
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
void CodeGen::Visit(const ASTWhileStmt *While) const {
  /// We want to avoid multiple labels with nested loops like a:
  /// L1: // First loop.
  /// L2: // Second loop.
  /// L3: // Third loop.
  if (!std::holds_alternative<GotoLabel>(Emitter.GetLast())) {
    Emitter.EmitGotoLabel(CurrentGotoLabel);
    ++CurrentGotoLabel;
  }
  EmitCondition(While->GetCondition().get(), CurrentGotoLabel);
  Jump *Fallback = Emitter.EmitJump(/*GotoLabel=*/0U);
  /// We will fall here if the condition was evaluated as true.
  Emitter.EmitGotoLabel(CurrentGotoLabel);
  unsigned BeginLabel = CurrentGotoLabel++ - 1;
  Jump *BreakJump = nullptr;
  Jump *ContinueJump = nullptr;
  EmitLoopBody(BreakJump, ContinueJump, While->GetBody().get());
  /// If break exists, we want to jump outside the loop.
  if (BreakJump)
    BreakJump->SetLabelNo(CurrentGotoLabel);
  /// If continue exists, we want to immediate jump again to the condition.
  if (ContinueJump)
    ContinueJump->SetLabelNo(BeginLabel);
  /// Jump again to condition.
  Emitter.EmitJump(BeginLabel);
  /// This a loop exit label.
  /// Example:
  ///     if 1 < 1 goto L1
  ///     goto L2
  /// L1: instr1
  ///     instr2
  /// L2: /* The end of loop. */
  Emitter.EmitGotoLabel(CurrentGotoLabel);
  Fallback->SetLabelNo(CurrentGotoLabel++);
}

} // namespace middleEnd
} // namespace weak