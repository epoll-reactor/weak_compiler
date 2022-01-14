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

using namespace weak::frontEnd;

namespace {

template <class... Ts> struct Overload : Ts... { using Ts::operator()...; };
template <class... Ts> Overload(Ts...) -> Overload<Ts...>;

} // namespace

namespace weak {
namespace middleEnd {

CodeGen::CodeGen(frontEnd::ASTNode *TheRootNode) : RootNode(TheRootNode) {}

void CodeGen::CreateCode() {
  VisitBaseNode(RootNode);
  Emitter.Dump();
}

CodeGen::AnyInstruction CodeGen::VisitBaseNode(const frontEnd::ASTNode *Node) {
  switch (Node->GetASTType()) {
  case ASTType::BASE_NODE:
    break;
  case ASTType::INTEGER_LITERAL:
    return Visit(static_cast<const ASTIntegerLiteral *>(Node));
  case ASTType::FLOATING_POINT_LITERAL:
    return Visit(static_cast<const ASTFloatingPointLiteral *>(Node));
  case ASTType::STRING_LITERAL:
    return Visit(static_cast<const ASTStringLiteral *>(Node));
  case ASTType::BOOLEAN_LITERAL:
    return Visit(static_cast<const ASTBooleanLiteral *>(Node));
  case ASTType::SYMBOL:
    return Visit(static_cast<const ASTSymbol *>(Node));
  case ASTType::VAR_DECL:
    return Visit(static_cast<const ASTVarDecl *>(Node));
  case ASTType::PARAMETER:
    break;
  case ASTType::BREAK_STMT:
    return Visit(static_cast<const ASTBreakStmt *>(Node));
  case ASTType::CONTINUE_STMT:
    return Visit(static_cast<const ASTContinueStmt *>(Node));
  case ASTType::BINARY:
    return Visit(static_cast<const ASTBinaryOperator *>(Node));
  case ASTType::PREFIX_UNARY: // Fall through.
  case ASTType::POSTFIX_UNARY:
    return Visit(static_cast<const ASTUnaryOperator *>(Node));
  case ASTType::IF_STMT:
    return Visit(static_cast<const ASTIfStmt *>(Node));
  case ASTType::FOR_STMT:
    return Visit(static_cast<const ASTForStmt *>(Node));
  case ASTType::WHILE_STMT:
    return Visit(static_cast<const ASTWhileStmt *>(Node));
  case ASTType::DO_WHILE_STMT:
    return Visit(static_cast<const ASTDoWhileStmt *>(Node));
  case ASTType::RETURN_STMT:
    return Visit(static_cast<const ASTReturnStmt *>(Node));
  case ASTType::COMPOUND_STMT:
    return Visit(static_cast<const ASTCompoundStmt *>(Node));
  case ASTType::FUNCTION_DECL:
    return Visit(static_cast<const ASTFunctionDecl *>(Node));
  case ASTType::FUNCTION_CALL:
    return Visit(static_cast<const ASTFunctionCall *>(Node));
  }
  DiagnosticError() << "Should not reach here.";
  UnreachablePoint();
}

CodeGen::AnyInstruction
CodeGen::Visit(const frontEnd::ASTCompoundStmt *Compound) {
  for (const auto &Stmt : Compound->GetStmts())
    VisitBaseNode(Stmt.get());

  return 0;
}

CodeGen::AnyInstruction
CodeGen::Visit(const frontEnd::ASTFunctionDecl *FunctionDecl) {
  VisitBaseNode(FunctionDecl->GetBody().get());

  return 0;
}

CodeGen::AnyInstruction
CodeGen::Visit(const frontEnd::ASTBinaryOperator *Binary) {
  auto LHS = VisitBaseNode(Binary->GetLHS().get());
  auto RHS = VisitBaseNode(Binary->GetRHS().get());

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

  return Emitter.GetInstructions().back();
}

CodeGen::AnyInstruction
CodeGen::Visit(const frontEnd::ASTIntegerLiteral *Integer) {
  return Integer->GetValue();
}

CodeGen::AnyInstruction CodeGen::Visit(const frontEnd::ASTVarDecl *VarDecl) {
  VisitBaseNode(VarDecl->GetDeclareBody().get());

  return 0;
}

CodeGen::AnyInstruction CodeGen::Visit(const frontEnd::ASTBooleanLiteral *) {
  return 0;
}
CodeGen::AnyInstruction CodeGen::Visit(const frontEnd::ASTBreakStmt *) {
  return 0;
}
CodeGen::AnyInstruction CodeGen::Visit(const frontEnd::ASTContinueStmt *) {
  return 0;
}
CodeGen::AnyInstruction CodeGen::Visit(const frontEnd::ASTDoWhileStmt *) {
  return 0;
}
CodeGen::AnyInstruction
CodeGen::Visit(const frontEnd::ASTFloatingPointLiteral *) {
  return 0;
}
CodeGen::AnyInstruction CodeGen::Visit(const frontEnd::ASTForStmt *) {
  return 0;
}
CodeGen::AnyInstruction CodeGen::Visit(const frontEnd::ASTFunctionCall *) {
  return 0;
}

CodeGen::AnyInstruction CodeGen::Visit(const frontEnd::ASTIfStmt *If) {
  auto Condition = VisitBaseNode(If->GetCondition().get());

  VisitBaseNode(If->GetThenBody().get());

  if (auto *Else = If->GetElseBody().get())
    VisitBaseNode(Else);

  return 0;
}

CodeGen::AnyInstruction CodeGen::Visit(const frontEnd::ASTReturnStmt *) {
  return 0;
}
CodeGen::AnyInstruction CodeGen::Visit(const frontEnd::ASTStringLiteral *) {
  return 0;
}
CodeGen::AnyInstruction CodeGen::Visit(const frontEnd::ASTSymbol *) {
  return 0;
}
CodeGen::AnyInstruction CodeGen::Visit(const frontEnd::ASTUnaryOperator *) {
  return 0;
}
CodeGen::AnyInstruction CodeGen::Visit(const frontEnd::ASTWhileStmt *) {
  return 0;
}

} // namespace middleEnd
} // namespace weak