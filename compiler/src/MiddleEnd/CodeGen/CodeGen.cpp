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

using namespace weak::frontEnd;

namespace {

template <class... Ts> struct Overload : Ts... { using Ts::operator()...; };
template <class... Ts> Overload(Ts...) -> Overload<Ts...>;

} // namespace

namespace weak {
namespace middleEnd {

CodeGen::CodeGen(frontEnd::ASTNode *TheRootNode)
    : RootNode(TheRootNode), Emitter(), LastInstruction(0), Instructions() {}

void CodeGen::CreateCode() {
  RootNode->Accept(this);
  Emitter.Dump();
}

void CodeGen::Visit(const frontEnd::ASTCompoundStmt *Compound) const {
  for (const auto &Stmt : Compound->GetStmts())
    Stmt->Accept(this);
}

void CodeGen::Visit(const frontEnd::ASTFunctionDecl *FunctionDecl) const {
  FunctionDecl->GetBody()->Accept(this);
}

void CodeGen::Visit(const frontEnd::ASTBinaryOperator *Binary) const {
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
}

void CodeGen::Visit(const frontEnd::ASTBooleanLiteral *) const {}
void CodeGen::Visit(const frontEnd::ASTBreakStmt *) const {}
void CodeGen::Visit(const frontEnd::ASTContinueStmt *) const {}
void CodeGen::Visit(const frontEnd::ASTDoWhileStmt *) const {}
void CodeGen::Visit(const frontEnd::ASTFloatingPointLiteral *) const {}
void CodeGen::Visit(const frontEnd::ASTForStmt *) const {}
void CodeGen::Visit(const frontEnd::ASTFunctionCall *) const {}

void CodeGen::Visit(const frontEnd::ASTIfStmt *) const {
  //  auto Condition = VisitBaseNode(If->GetCondition().get());
  //
  //  VisitBaseNode(If->GetThenBody().get());
  //
  //  if (auto *Else = If->GetElseBody().get())
  //    VisitBaseNode(Else);
  //
  //  return 0;
}

void CodeGen::Visit(const frontEnd::ASTReturnStmt *) const {}
void CodeGen::Visit(const frontEnd::ASTStringLiteral *) const {}
void CodeGen::Visit(const frontEnd::ASTSymbol *) const {}
void CodeGen::Visit(const frontEnd::ASTUnaryOperator *) const {}
void CodeGen::Visit(const frontEnd::ASTWhileStmt *) const {}

} // namespace middleEnd
} // namespace weak