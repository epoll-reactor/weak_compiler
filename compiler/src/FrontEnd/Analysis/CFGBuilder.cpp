/* CFGBuilder.cpp - Transformer from AST to CFG.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/Analysis/CFGBuilder.hpp"
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
#include <cassert>

using namespace weak::frontEnd;

template <typename Base, typename Derived>
static auto StaticUniquePtrCast(std::unique_ptr<Base> &&Ptr) {
  return std::unique_ptr<Derived, std::default_delete<Derived>>(
      static_cast<Derived *>(Ptr.release()), std::default_delete<Derived>{});
}

namespace {

/*! This is helper class to link CFG blocks together.
 *
 *  For example, we have `if` and `while` statements. We need to set
 *  `while` condition as successor of `if` false condition or else block.
 *
 *  This class links loop and trivial statements together in similar way.
 */
class CFGBlocksLinker {
public:
  CFGBlocksLinker(std::vector<std::shared_ptr<CFGBlock>> &UnlinkedBlocks)
      : Blocks(UnlinkedBlocks) {}

  void LinkAll() {
    for (size_t It = 1; Blocks.size() >= 2 && It < Blocks.size(); ++It) {
      switch (Blocks.at(It - 1)->GetStmtType()) {
      case CFGBlock::Stmt::Trivial:
        LinkTrivialStmt(It);
        break;
      case CFGBlock::Stmt::If:
        LinkIfStmt(It);
        break;
      case CFGBlock::Stmt::For:
        LinkForStmt(It);
        break;
      case CFGBlock::Stmt::While:
        LinkWhileStmt(It);
        break;
      case CFGBlock::Stmt::DoWhile:
        LinkDoWhileStmt(It);
        break;
      default:
        break;
      }
    }
  }

private:
  void LinkTrivialStmt(size_t Position) {
    auto &TrivialStmt = Blocks.at(Position - 1);
    auto &NextStmt = Blocks.at(Position);

    TrivialStmt->AddSequentialSuccessor(NextStmt);
  }

  void LinkIfStmt(size_t Position) {
    auto &IfCondition = Blocks.at(Position - 1);
    auto &IfBody = IfCondition->GetSequentialSuccessor();
    auto &ElseBody = IfCondition->GetConditionalSuccessor();
    auto &AfterIfBlock = Blocks.at(Position);

    assert(IfCondition);
    assert(IfBody);

    if (ElseBody) {
      IfBody->AddSequentialSuccessor(AfterIfBlock);
      ElseBody->AddSequentialSuccessor(AfterIfBlock);
    } else {
      IfBody->AddSequentialSuccessor(AfterIfBlock);
      IfCondition->AddConditionSuccessor(AfterIfBlock);
    }
  }

  void LinkForStmt(size_t Position) {
    auto &ForInit = Blocks.at(Position - 1);
    auto &ForCondition = ForInit->GetSequentialSuccessor();
    auto &AfterForBlock = Blocks.at(Position);

    assert(AfterForBlock);
    assert(ForCondition);
    ForCondition->AddConditionSuccessor(AfterForBlock);
  }

  void LinkWhileStmt(size_t Position) {
    auto &WhileCondition = Blocks.at(Position - 1);
    auto &AfterWhileBlock = Blocks.at(Position);

    assert(WhileCondition);
    WhileCondition->AddConditionSuccessor(AfterWhileBlock);
  }

  void LinkDoWhileStmt(size_t Position) {
    auto &DoWhileCondition = Blocks.at(Position - 1);
    auto &AfterDoWhileBlock = Blocks.at(Position);

    DoWhileCondition->AddConditionSuccessor(AfterDoWhileBlock);
  }

  std::vector<std::shared_ptr<CFGBlock>> &Blocks;
};

} // namespace

namespace weak {
namespace frontEnd {

CFGBuilder::CFGBuilder(std::unique_ptr<ASTNode> &&TheRootNode)
    : RootNode(std::move(TheRootNode)) {}

std::vector<std::shared_ptr<CFGBlock>> CFGBuilder::BuildCFG() {
  std::vector<std::shared_ptr<CFGBlock>> FunctionGraphs;

  FunctionGraphs.push_back(Visit(std::move(RootNode)));

  return FunctionGraphs;
}

std::shared_ptr<CFGBlock> CFGBuilder::Visit(std::unique_ptr<ASTNode> &&Node) {
  switch (Node->GetASTType()) {
  case ASTType::INTEGER_LITERAL:
    return Visit(
        StaticUniquePtrCast<ASTNode, ASTIntegerLiteral>(std::move(Node)));

  case ASTType::FLOATING_POINT_LITERAL:
    return Visit(
        StaticUniquePtrCast<ASTNode, ASTFloatingPointLiteral>(std::move(Node)));

  case ASTType::STRING_LITERAL:
    return Visit(
        StaticUniquePtrCast<ASTNode, ASTStringLiteral>(std::move(Node)));

  case ASTType::BOOLEAN_LITERAL:
    return Visit(
        StaticUniquePtrCast<ASTNode, ASTBooleanLiteral>(std::move(Node)));

  case ASTType::SYMBOL:
    return Visit(StaticUniquePtrCast<ASTNode, ASTSymbol>(std::move(Node)));

  case ASTType::VAR_DECL:
    return Visit(StaticUniquePtrCast<ASTNode, ASTVarDecl>(std::move(Node)));

  case ASTType::BREAK_STMT:
    return Visit(StaticUniquePtrCast<ASTNode, ASTBreakStmt>(std::move(Node)));

  case ASTType::CONTINUE_STMT:
    return Visit(
        StaticUniquePtrCast<ASTNode, ASTContinueStmt>(std::move(Node)));

  case ASTType::BINARY:
    return Visit(
        StaticUniquePtrCast<ASTNode, ASTBinaryOperator>(std::move(Node)));

  case ASTType::PREFIX_UNARY: // Fall through.
  case ASTType::POSTFIX_UNARY:
    return Visit(
        StaticUniquePtrCast<ASTNode, ASTUnaryOperator>(std::move(Node)));

  case ASTType::IF_STMT:
    return Visit(StaticUniquePtrCast<ASTNode, ASTIfStmt>(std::move(Node)));

  case ASTType::FOR_STMT:
    return Visit(StaticUniquePtrCast<ASTNode, ASTForStmt>(std::move(Node)));

  case ASTType::WHILE_STMT:
    return Visit(StaticUniquePtrCast<ASTNode, ASTWhileStmt>(std::move(Node)));

  case ASTType::DO_WHILE_STMT:
    return Visit(StaticUniquePtrCast<ASTNode, ASTDoWhileStmt>(std::move(Node)));

  case ASTType::RETURN_STMT:
    return Visit(StaticUniquePtrCast<ASTNode, ASTReturnStmt>(std::move(Node)));

  case ASTType::COMPOUND_STMT:
    return Visit(
        StaticUniquePtrCast<ASTNode, ASTCompoundStmt>(std::move(Node)));

  case ASTType::FUNCTION_DECL:
    return Visit(
        StaticUniquePtrCast<ASTNode, ASTFunctionDecl>(std::move(Node)));

  case ASTType::FUNCTION_CALL:
    return Visit(
        StaticUniquePtrCast<ASTNode, ASTFunctionCall>(std::move(Node)));

  default:
    DiagnosticError() << "Unexpected statement reached while building the CFG.";
    UnreachablePoint();
  }
}

std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTBinaryOperator> &&Binary) {
  auto CurrentBlock = CreateCFGBlock();
  CurrentBlock->AddStatement(std::move(Binary));
  return CurrentBlock;
}

std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTBreakStmt> &&Break) {
  auto CurrentBlock = CreateCFGBlock();
  assert(CurrentBlock);
  CurrentBlock->AddStatement(std::move(Break));
  return CurrentBlock;
}

std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTCompoundStmt> &&Compound) {
  std::vector<std::shared_ptr<CFGBlock>> Blocks;

  for (auto &&Stmt : Compound->GetStmts())
    Blocks.push_back(Visit(std::move(Stmt)));

  /// At the moment, we have list of unlinked blocks, so solve this problem.
  CFGBlocksLinker Linker(Blocks);
  Linker.LinkAll();

  /// We return the first block because all others are linked with it and
  /// continue their life time.
  return Blocks.front();
}

std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTContinueStmt> &&Continue) {
  auto CurrentBlock = CreateCFGBlock();
  CurrentBlock->AddStatement(std::move(Continue));
  return CurrentBlock;
}

/*  do { body(); } while (cond);
 *
 *          +----------+
 *          |  body()  |<----
 *          +----------+     \
 *               |           |
 *          +----------+     |
 *          |   cond   |     |
 *          +----------+     |
 *             /    \        |
 *         no /      \ yes   |
 *           |        \     /
 *           v         -----
 *    +------------+
 *    | after loop |
 *    +------------+
 */
std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTDoWhileStmt> &&DoWhile) {
  auto ConditionBlock = CreateCFGBlock();
  ConditionBlock->SetStmtType(CFGBlock::Stmt::DoWhile);
  ConditionBlock->AddStatement(DoWhile->GetCondition());

  auto BodyBlock = Visit(DoWhile->GetBody());
  ConditionBlock->AddSequentialSuccessor(BodyBlock);

  /// Don't link this to simplify tree traversal.
  /// ConditionBlock->AddSequentialSuccessor(BodyBlock);

  return ConditionBlock;
}

std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTFloatingPointLiteral> &&Float) {
  auto CurrentBlock = CreateCFGBlock();
  CurrentBlock->AddStatement(std::move(Float));
  return CurrentBlock;
}

/*! for (init; cond; inc) { body }
 *
 *                 +---------+
 *                 |   init  |
 *                 +---------+
 *                      |
 *                 +---------+
 *                 |   cond  |<-----------
 *                 +---------+            \
 *                no /    \ yes           |
 *                  /      \          +--------+
 *                 /        \         |   inc  |
 *                v          v        +--------+
 *         +------------+  +---------+    |
 *         | after loop |  |   body  |    |
 *         +------------+  +---------+    |
 *                            \___________/
 */
std::shared_ptr<CFGBlock> CFGBuilder::Visit(std::unique_ptr<ASTForStmt> &&For) {
  auto &&ForInit = For->GetInit();
  auto &&ForCondition = For->GetCondition();
  auto &&ForIncrement = For->GetIncrement();
  auto &&ForBody = For->GetBody();

  auto ForInitBlock = CreateCFGBlock();
  auto ForConditionBlock = CreateCFGBlock();
  auto ForIncrementBlock = CreateCFGBlock();

  ForInitBlock->SetStmtType(CFGBlock::For);

  ForInitBlock->AddStatement(std::move(ForInit));
  ForConditionBlock->AddStatement(std::move(ForCondition));
  ForIncrementBlock->AddStatement(std::move(ForIncrement));

  auto ForBodyBlock = Visit(std::move(ForBody));

  ForInitBlock->AddSequentialSuccessor(ForConditionBlock);
  ForConditionBlock->AddSequentialSuccessor(ForIncrementBlock);
  ForIncrementBlock->AddSequentialSuccessor(ForBodyBlock);
  /// Don't link this to simplify tree traversal.
  /// ForIncrementBlock->AddSequentialSuccessor(ForConditionBlock);

  return ForInitBlock;
}

std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTFunctionCall> &&FunctionCall) {
  auto CurrentBlock = CreateCFGBlock();
  CurrentBlock->AddStatement(std::move(FunctionCall));
  return CurrentBlock;
}

std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTFunctionDecl> &&FunctionDecl) {
  return Visit(FunctionDecl->GetBody());
}

/* if (cond) { body }
 * if (cond) { body } else { body }
 *
 *            +--------+                 +--------+
 *            |  cond  |                 |  cond  |
 *            +--------+                 +--------+
 *              /   \                      /   \
 *          no /     \ yes             no /     \ yes
 *            |   +--------+       +--------+  +--------+
 *            |   |  body  |       |  body  |  |  body  |
 *            |   +--------+       +--------+  +--------+
 *            |       |                  \       /
 *             \     /                    \     /
 *              \   /                      \   /
 *           +----------+              +----------+
 *           | after if |              | after if |
 *           +----------+              +----------+
 *
 */
std::shared_ptr<CFGBlock> CFGBuilder::Visit(std::unique_ptr<ASTIfStmt> &&If) {
  auto ConditionBlock = CreateCFGBlock();
  ConditionBlock->SetStmtType(CFGBlock::Stmt::If);
  ConditionBlock->AddStatement(If->GetCondition());

  auto ThenBlock = Visit(If->GetThenBody());
  ConditionBlock->AddSequentialSuccessor(ThenBlock);

  if (If->GetElseBody()) {
    auto ElseBlock = Visit(If->GetElseBody());
    ConditionBlock->AddConditionSuccessor(ElseBlock);
  }

  return ConditionBlock;
}

std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTIntegerLiteral> &&Integer) {
  auto CurrentBlock = CreateCFGBlock();
  CurrentBlock->AddStatement(std::move(Integer));
  return CurrentBlock;
}

std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTReturnStmt> &&Return) {
  auto CurrentBlock = CreateCFGBlock();
  CurrentBlock->AddStatement(std::move(Return));
  return CurrentBlock;
}

std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTStringLiteral> &&String) {
  auto CurrentBlock = CreateCFGBlock();
  CurrentBlock->AddStatement(std::move(String));
  return CurrentBlock;
}

std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTSymbol> &&Symbol) {
  auto CurrentBlock = CreateCFGBlock();
  CurrentBlock->AddStatement(std::move(Symbol));
  return CurrentBlock;
}

std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTUnaryOperator> &&Unary) {
  auto CurrentBlock = CreateCFGBlock();
  CurrentBlock->AddStatement(std::move(Unary));
  return CurrentBlock;
}

std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTVarDecl> &&VarDecl) {
  auto CurrentBlock = CreateCFGBlock();
  CurrentBlock->AddStatement(std::move(VarDecl));
  return CurrentBlock;
}

/*  while (cond) { body(); }
 *
 *               +--------+
 *               |  cond  |<-----
 *               +--------+      \
 *                 /    \        |
 *             no /      v yes   |
 *               |   +--------+  |
 *               |   | body() |  |
 *               |   +--------+  |
 *               |        |      |
 *               |        \______/
 *               v
 *        +------------+
 *        | after loop |
 *        +------------+
 */
std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTWhileStmt> &&While) {
  auto ConditionBlock = CreateCFGBlock();
  ConditionBlock->SetStmtType(CFGBlock::Stmt::While);
  ConditionBlock->AddStatement(While->GetCondition());

  auto BodyBlock = Visit(While->GetBody());
  ConditionBlock->AddSequentialSuccessor(BodyBlock);
  /// Don't link this to simplify tree traversal.
  /// BodyBlock->AddSequentialSuccessor(ConditionBlock);

  return ConditionBlock;
}

std::shared_ptr<CFGBlock>
CFGBuilder::Visit(std::unique_ptr<ASTBooleanLiteral> &&Boolean) {
  auto CurrentBlock = CreateCFGBlock();
  CurrentBlock->AddStatement(std::move(Boolean));
  return CurrentBlock;
}

std::shared_ptr<CFGBlock> CFGBuilder::CreateCFGBlock() {
  return std::make_shared<CFGBlock>();
}

} // namespace frontEnd
} // namespace weak