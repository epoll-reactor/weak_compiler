/* CodeGen.cpp - LLVM IR generator.
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
#include "FrontEnd/AST/ASTReturnStmt.hpp"
#include "FrontEnd/AST/ASTStringLiteral.hpp"
#include "FrontEnd/AST/ASTSymbol.hpp"
#include "FrontEnd/AST/ASTUnaryOperator.hpp"
#include "FrontEnd/AST/ASTVarDecl.hpp"
#include "FrontEnd/AST/ASTWhileStmt.hpp"
#include "MiddleEnd/CodeGen/TargetCodeBuilder.hpp"
#include "MiddleEnd/CodeGen/TypeResolver.hpp"
#include "Utility/Diagnostic.hpp"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

namespace weak {
namespace middleEnd {

CodeGen::CodeGen(frontEnd::ASTNode *TheRoot)
    : Root(TheRoot), LastEmitted(nullptr), LLVMCtx(),
      LLVMModule("LLVM Module", LLVMCtx), CodeBuilder(LLVMCtx),
      IsReturnValue(false) {}

void CodeGen::CreateCode(std::string_view ObjectFilePath) {
  Root->Accept(this);

  llvm::outs() << "Compiled code:\n" << ToString();

  TargetCodeBuilder TargetBuilder(LLVMModule, ObjectFilePath);
  TargetBuilder.Build();
}

void CodeGen::Visit(const frontEnd::ASTBooleanLiteral *Stmt) const {
  llvm::APInt Int(8, Stmt->GetValue(), false);
  LastEmitted = llvm::ConstantInt::get(LLVMCtx, Int);
}

void CodeGen::Visit(const frontEnd::ASTIntegerLiteral *Stmt) const {
  llvm::APInt Int(32, Stmt->GetValue(), false);
  LastEmitted = llvm::ConstantInt::get(LLVMCtx, Int);
}

void CodeGen::Visit(const frontEnd::ASTFloatingPointLiteral *Stmt) const {
  llvm::APFloat Float(Stmt->GetValue());
  LastEmitted = llvm::ConstantFP::get(LLVMCtx, Float);
}

void CodeGen::Visit(const frontEnd::ASTBinaryOperator *Stmt) const {
  /// \todo: Make type checking, e.g. decide how to handle
  ///        expressions such as 1 + 2.0.
  Stmt->GetLHS()->Accept(this);
  llvm::Value *L = LastEmitted;
  Stmt->GetRHS()->Accept(this);
  llvm::Value *R = LastEmitted;

  if (!L || !R) {
    return;
  }

  using frontEnd::TokenType;
  switch (Stmt->GetOperation()) {
  case TokenType::ASSIGN: {
    auto *Assignment =
        static_cast<const frontEnd::ASTSymbol *>(Stmt->GetLHS().get());
    CodeBuilder.CreateStore(R, VariablesMapping[Assignment->GetName()]);
    break;
  }
  case TokenType::PLUS:
    LastEmitted = CodeBuilder.CreateAdd(L, R);
    break;
  case TokenType::MINUS:
    LastEmitted = CodeBuilder.CreateSub(L, R);
    break;
  case TokenType::STAR:
    LastEmitted = CodeBuilder.CreateMul(L, R);
    break;
  case TokenType::SLASH:
    LastEmitted = CodeBuilder.CreateSDiv(L, R);
    break;
  case TokenType::LT:
    LastEmitted = CodeBuilder.CreateICmpSLT(L, R);
    break;
  default:
    LastEmitted = nullptr;
    weak::CompileError() << "Invalid binary operator";
    break;
  }
}

void CodeGen::Visit(const frontEnd::ASTUnaryOperator *Stmt) const {
  Stmt->GetOperand()->Accept(this);

  llvm::APInt Int(
      /*numBits=*/32,
      /*val=*/1,
      /*isSigned=*/false);
  llvm::Value *Step = llvm::ConstantInt::get(LLVMCtx, Int);

  if (Stmt->GetOperand()->GetASTType() != frontEnd::ASTType::SYMBOL) {
    weak::CompileError() << "Variable as argument of unary operator expected";
    return;
  }

  auto *SymbolOperand =
      static_cast<const frontEnd::ASTSymbol *>(Stmt->GetOperand().get());

  using frontEnd::TokenType;
  switch (Stmt->GetOperation()) {
  case TokenType::INC: {
    LastEmitted = CodeBuilder.CreateAdd(LastEmitted, Step);
    // If we're expecting that unary operand is a variable,
    // the store operation was performed, when variable was
    // created or assigned, so we can safely do store.
    CodeBuilder.CreateStore(LastEmitted,
                            VariablesMapping[SymbolOperand->GetName()]);
    break;
  }
  case TokenType::DEC: {
    LastEmitted = CodeBuilder.CreateSub(LastEmitted, Step);
    CodeBuilder.CreateStore(LastEmitted,
                            VariablesMapping[SymbolOperand->GetName()]);
    break;
  }
  default: {
    unsigned LineNo = Stmt->GetLineNo();
    unsigned ColumnNo = Stmt->GetColumnNo();
    weak::CompileError(LineNo, ColumnNo) << "Unknown unary operator.";
    break;
  }
  } // switch
}

void CodeGen::Visit(const frontEnd::ASTForStmt *Stmt) const {
  /// \todo: Generate code with respect to empty for parameters,
  ///        e.g for (;;), or for(int i = 0; ; ++i). Also
  ///        break,continue statements should be implemented.
  Stmt->GetInit()->Accept(this);

  llvm::Function *Func = CodeBuilder.GetInsertBlock()->getParent();

  llvm::BasicBlock *ForCondBB =
      llvm::BasicBlock::Create(LLVMCtx, "for.cond", Func);
  llvm::BasicBlock *ForBodyBB =
      llvm::BasicBlock::Create(LLVMCtx, "for.body", Func);
  llvm::BasicBlock *ForEndBB =
      llvm::BasicBlock::Create(LLVMCtx, "for.end", Func);

  CodeBuilder.CreateBr(ForCondBB);
  CodeBuilder.SetInsertPoint(ForCondBB);

  Stmt->GetCondition()->Accept(this);
  CodeBuilder.CreateCondBr(LastEmitted, ForBodyBB, ForEndBB);
  CodeBuilder.SetInsertPoint(ForBodyBB);
  Stmt->GetBody()->Accept(this);
  Stmt->GetIncrement()->Accept(this);
  CodeBuilder.CreateBr(ForCondBB);
  CodeBuilder.SetInsertPoint(ForEndBB);
}

void CodeGen::Visit(const frontEnd::ASTIfStmt *Stmt) const {
  Stmt->GetCondition()->Accept(this);
  llvm::Value *Condition = LastEmitted;

  Condition = CodeBuilder.CreateICmpNE(
      Condition, llvm::ConstantInt::get(LLVMCtx, llvm::APInt(32, 0, false)),
      "condition");

  llvm::Function *Func = CodeBuilder.GetInsertBlock()->getParent();

  llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(LLVMCtx, "if.then", Func);
  llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(LLVMCtx, "if.else");
  llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(LLVMCtx, "if.end");

  if (Stmt->GetElseBody())
    CodeBuilder.CreateCondBr(Condition, ThenBB, ElseBB);
  else
    CodeBuilder.CreateCondBr(Condition, ThenBB, MergeBB);

  CodeBuilder.SetInsertPoint(ThenBB);
  Stmt->GetThenBody()->Accept(this);
  CodeBuilder.CreateBr(MergeBB);

  if (!Stmt->GetElseBody()) {
    Func->getBasicBlockList().push_back(MergeBB);
    CodeBuilder.SetInsertPoint(MergeBB);
    return;
  }

  Func->getBasicBlockList().push_back(ElseBB);
  CodeBuilder.SetInsertPoint(ElseBB);

  Stmt->GetElseBody()->Accept(this);
  CodeBuilder.CreateBr(MergeBB);

  Func->getBasicBlockList().push_back(MergeBB);
  CodeBuilder.SetInsertPoint(MergeBB);
}

namespace {

/// Create part (without body and return type) of function IR from AST.
class FunctionBuilder {
public:
  FunctionBuilder(llvm::LLVMContext &TheCtx, llvm::Module &TheModule,
                  const frontEnd::ASTFunctionDecl *TheDecl)
      : Ctx(TheCtx), Module(TheModule), Decl(TheDecl) {}

  llvm::Function *Build() {
    llvm::FunctionType *Signature = CreateSignature();
    llvm::Function *Func = llvm::Function::Create(
        Signature, llvm::Function::ExternalLinkage, Decl->GetName(), &Module);

    unsigned Idx{0U};
    for (llvm::Argument &Arg : Func->args())
      Arg.setName(ExtractSymbol(Decl->GetArguments()[Idx++].get()));

    return Func;
  }

private:
  llvm::FunctionType *CreateSignature() {
    middleEnd::TypeResolver TypeResolver(Ctx);
    llvm::SmallVector<llvm::Type *, 16> ArgTypes;

    for (const auto &Arg : Decl->GetArguments())
      ArgTypes.push_back(TypeResolver.ResolveExceptVoid(Arg.get()));

    llvm::FunctionType *Signature = llvm::FunctionType::get(
        // Return type.
        TypeResolver.Resolve(Decl->GetReturnType()),
        // Arguments.
        ArgTypes,
        // Variadic parameters?
        false);

    return Signature;
  }

  static const std::string &ExtractSymbol(const frontEnd::ASTNode *Node) {
    const auto *VarDecl = static_cast<const frontEnd::ASTVarDecl *>(Node);
    return VarDecl->GetSymbolName();
  }

  llvm::LLVMContext &Ctx;
  llvm::Module &Module;
  const frontEnd::ASTFunctionDecl *Decl;
};

} // namespace

void CodeGen::Visit(const frontEnd::ASTFunctionDecl *Decl) const {
  FunctionBuilder FunctionBuilder(LLVMCtx, LLVMModule, Decl);

  llvm::Function *Func = FunctionBuilder.Build();
  llvm::BasicBlock *CFGBlock = llvm::BasicBlock::Create(LLVMCtx, "entry", Func);
  CodeBuilder.SetInsertPoint(CFGBlock);

  VariablesMapping.clear();
  for (auto &Arg : Func->args())
    VariablesMapping.emplace(Arg.getName(), &Arg);

  Decl->GetBody()->Accept(this);

  if (IsReturnValue) {
    llvm::verifyFunction(*Func);
    LastEmitted = Func;
    IsReturnValue = false;
  } else {
    Func->eraseFromParent();
    LastEmitted = nullptr;
  }
}

void CodeGen::Visit(const frontEnd::ASTFunctionCall *Stmt) const {
  llvm::Function *Callee = LLVMModule.getFunction(Stmt->GetName());
  if (!Callee) {
    weak::CompileError() << "Unknown function: " << Stmt->GetName();
    return;
  }

  const auto &FunArgs = Stmt->GetArguments();

  if (Callee->arg_size() != FunArgs.size()) {
    weak::CompileError() << "Arguments size mismatch (" << Callee->arg_size()
                         << " vs " << FunArgs.size() << ")";
    return;
  }

  llvm::SmallVector<llvm::Value *, 16> Args;
  for (size_t I = 0; I < FunArgs.size(); ++I) {
    FunArgs.at(I)->Accept(this);
    Args.push_back(LastEmitted);
    if (!Args.back())
      return;
  }

  LastEmitted = CodeBuilder.CreateCall(Callee, Args);
}

void CodeGen::Visit(const frontEnd::ASTSymbol *Stmt) const {
  llvm::Value *V = VariablesMapping[Stmt->GetName()];
  if (!V) {
    weak::CompileError() << "Unknown variable name: " << Stmt->GetName();
    return;
  }

  llvm::AllocaInst *Alloca = llvm::dyn_cast<llvm::AllocaInst>(V);
  if (Alloca)
    // Variable.
    LastEmitted = CodeBuilder.CreateLoad(llvm::Type::getInt32Ty(LLVMCtx),
                                         Alloca, Stmt->GetName());
  else
    // Function Parameter.
    LastEmitted = V;
}

void CodeGen::Visit(const frontEnd::ASTCompoundStmt *Stmts) const {
  for (const auto &Stmt : Stmts->GetStmts()) {
    Stmt->Accept(this);
  }
}

void CodeGen::Visit(const frontEnd::ASTReturnStmt *Stmt) const {
  Stmt->GetOperand()->Accept(this);
  CodeBuilder.CreateRet(LastEmitted);
  IsReturnValue = true;
}

void CodeGen::Visit(const frontEnd::ASTVarDecl *Decl) const {
  Decl->GetDeclareBody()->Accept(this);

  /// Alloca needed to be able to store mutable variables.
  /// We should also do load and store before and after
  /// every use of Alloca variable.
  if (VariablesMapping.count(Decl->GetSymbolName()) != 0) {
    llvm::Value *Alloca = VariablesMapping[Decl->GetSymbolName()];
    CodeBuilder.CreateStore(LastEmitted, Alloca);
  } else {
    TypeResolver TypeResolver(LLVMCtx);
    llvm::AllocaInst *Alloca = CodeBuilder.CreateAlloca(
        TypeResolver.ResolveExceptVoid(Decl->GetDataType()), nullptr,
        Decl->GetSymbolName());
    CodeBuilder.CreateStore(LastEmitted, Alloca);
    VariablesMapping.emplace(Decl->GetSymbolName(), Alloca);
  }
}

std::string CodeGen::ToString() {
  std::string Result;

  for (const auto &F : LLVMModule.getFunctionList()) {
    llvm::raw_string_ostream Stream(Result);
    Stream << F;
    Stream << '\n';
    Stream.flush();
  }

  return Result;
}

} // namespace middleEnd
} // namespace weak