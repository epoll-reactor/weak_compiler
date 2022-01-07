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
#include <cassert>

using namespace weak::frontEnd;

template <typename Base, typename Derived>
static auto StaticUniquePtrCast(std::unique_ptr<Base> &&Ptr) {
  return std::unique_ptr<Derived, std::default_delete<Derived>>(
      static_cast<Derived *>(Ptr.release()), std::default_delete<Derived>{});
}

namespace {

class CFGBlocksLinker {
public:
  CFGBlocksLinker(std::shared_ptr<CFGBlock> &TheTarget,
                  std::shared_ptr<CFGBlock> &TheSuccessor)
      : Target(TheTarget), Successor(TheSuccessor) {}

  /*! Trivial considered statements: literals, unary, binary, return, symbol,
   *  var decl, break, continue, function call.
   *
   *  Other: if, for, while, do-while.
   *
   */
  void Link() {}

private:
  [[maybe_unused]] std::shared_ptr<CFGBlock> &Target;
  [[maybe_unused]] std::shared_ptr<CFGBlock> &Successor;
};

} // namespace

namespace weak {
namespace frontEnd {

CFGBuilder::CFGBuilder(std::unique_ptr<ASTNode> &&TheRootNode)
    : RootNode(std::move(TheRootNode)) {}

std::vector<std::shared_ptr<CFGBlock>> CFGBuilder::BuildCFG() {
  VisitBaseNode(std::move(RootNode));
  return FunctionGraphs;
}

void CFGBuilder::VisitBaseNode(std::unique_ptr<ASTNode> &&Node) {
  switch (Node->GetASTType()) {
  case ASTType::BASE_NODE:
    break;
  case ASTType::INTEGER_LITERAL:
    Visit(StaticUniquePtrCast<ASTNode, ASTIntegerLiteral>(std::move(Node)));
    break;
  case ASTType::FLOATING_POINT_LITERAL:
    Visit(
        StaticUniquePtrCast<ASTNode, ASTFloatingPointLiteral>(std::move(Node)));
    break;
  case ASTType::STRING_LITERAL:
    Visit(StaticUniquePtrCast<ASTNode, ASTStringLiteral>(std::move(Node)));
    break;
  case ASTType::BOOLEAN_LITERAL:
    Visit(StaticUniquePtrCast<ASTNode, ASTBooleanLiteral>(std::move(Node)));
    break;
  case ASTType::SYMBOL:
    Visit(StaticUniquePtrCast<ASTNode, ASTSymbol>(std::move(Node)));
    break;
  case ASTType::VAR_DECL:
    Visit(StaticUniquePtrCast<ASTNode, ASTVarDecl>(std::move(Node)));
    break;
  case ASTType::PARAMETER:
    break;
  case ASTType::BREAK_STMT:
    Visit(StaticUniquePtrCast<ASTNode, ASTBreakStmt>(std::move(Node)));
    break;
  case ASTType::CONTINUE_STMT:
    Visit(StaticUniquePtrCast<ASTNode, ASTContinueStmt>(std::move(Node)));
    break;
  case ASTType::BINARY:
    Visit(StaticUniquePtrCast<ASTNode, ASTBinaryOperator>(std::move(Node)));
    break;
  case ASTType::PREFIX_UNARY: // Fall through.
  case ASTType::POSTFIX_UNARY:
    Visit(StaticUniquePtrCast<ASTNode, ASTUnaryOperator>(std::move(Node)));
    break;
  case ASTType::IF_STMT:
    Visit(StaticUniquePtrCast<ASTNode, ASTIfStmt>(std::move(Node)));
    break;
  case ASTType::FOR_STMT:
    Visit(StaticUniquePtrCast<ASTNode, ASTForStmt>(std::move(Node)));
    break;
  case ASTType::WHILE_STMT:
    Visit(StaticUniquePtrCast<ASTNode, ASTWhileStmt>(std::move(Node)));
    break;
  case ASTType::DO_WHILE_STMT:
    Visit(StaticUniquePtrCast<ASTNode, ASTDoWhileStmt>(std::move(Node)));
    break;
  case ASTType::RETURN_STMT:
    Visit(StaticUniquePtrCast<ASTNode, ASTReturnStmt>(std::move(Node)));
    break;
  case ASTType::COMPOUND_STMT:
    Visit(StaticUniquePtrCast<ASTNode, ASTCompoundStmt>(std::move(Node)));
    break;
  case ASTType::FUNCTION_DECL:
    Visit(StaticUniquePtrCast<ASTNode, ASTFunctionDecl>(std::move(Node)));
    break;
  case ASTType::FUNCTION_CALL:
    Visit(StaticUniquePtrCast<ASTNode, ASTFunctionCall>(std::move(Node)));
    break;
  }
}

void CFGBuilder::Visit(std::unique_ptr<ASTBinaryOperator> &&Binary) {
  CurrentBlock->AddStatement(std::move(Binary));
}

void CFGBuilder::Visit(std::unique_ptr<ASTBooleanLiteral> &&Boolean) {
  CurrentBlock->AddStatement(std::move(Boolean));
}

void CFGBuilder::Visit(std::unique_ptr<ASTBreakStmt> &&Break) {
  CurrentBlock->AddStatement(std::move(Break));
}

///\TODO implement linking of CFG blocks based on stmt type and it's successor.
void CFGBuilder::Visit(std::unique_ptr<ASTCompoundStmt> &&Compound) {
  std::vector<std::shared_ptr<CFGBlock>> BlockGraphs;

  for (auto &&Statement : Compound->GetStmts()) {
    CurrentBlock = std::make_shared<CFGBlock>();
    VisitBaseNode(std::move(Statement));
    BlockGraphs.push_back(std::move(CurrentBlock));
  }

  for (size_t It = 1; BlockGraphs.size() >= 2 && It < BlockGraphs.size();
       ++It) {
    /// E.g we have (binary) (if) (binary), then we should supplement links
    /// chain from partially formed CFG node:
    ///      (binary)(1) ->        (if)
    ///   (if-condition) -> (binary)(2)
    ///        (if-body) -> (binary)(2)
    CFGBlocksLinker Linker(BlockGraphs[It - 1], BlockGraphs[It]);
    Linker.Link();
  }
}

void CFGBuilder::Visit(std::unique_ptr<ASTContinueStmt> &&Continue) {
  CurrentBlock->AddStatement(std::move(Continue));
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
void CFGBuilder::Visit(std::unique_ptr<ASTDoWhileStmt> &&) {}

void CFGBuilder::Visit(std::unique_ptr<ASTFloatingPointLiteral> &&Float) {
  CurrentBlock->AddStatement(std::move(Float));
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
void CFGBuilder::Visit(std::unique_ptr<ASTForStmt> &&For) {
  CurrentBlock->AddStatement(For->GetInit());

  auto InitBlock = std::make_shared<CFGBlock>();
  InitBlock->AddStatement(For->GetInit());

  auto ConditionBlock = std::make_shared<CFGBlock>();
  ConditionBlock->AddStatement(For->GetCondition());

  auto IncrementBlock = std::make_shared<CFGBlock>();
  IncrementBlock->AddStatement(For->GetIncrement());

  /// This should be handled recursively...
  [[maybe_unused]] auto Body = std::make_shared<CFGBlock>();
}

void CFGBuilder::Visit(std::unique_ptr<ASTFunctionDecl> &&FunctionDecl) {
  FunctionGraphs.emplace_back(std::make_shared<CFGBlock>());
  Visit(FunctionDecl->GetBody());
}

void CFGBuilder::Visit(std::unique_ptr<ASTFunctionCall> &&Call) {
  CurrentBlock->AddStatement(std::move(Call));
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
void CFGBuilder::Visit(std::unique_ptr<ASTIfStmt> &&If) {
  CurrentBlock->AddStatement(If->GetCondition());

  auto IfBodyBlock = std::make_shared<CFGBlock>();
  /// This should be handled recursively...
  IfBodyBlock->AddStatement(If->GetThenBody());

  if (If->GetElseBody()) {
    auto ElseBodyBlock = std::make_shared<CFGBlock>();
    /// This should be handled recursively...
    ElseBodyBlock->AddStatement(If->GetElseBody());
    /// If-Else body successors are configured externally.
    //  if (NextBlock) {
    //    IfBodyBlock->AddSequentialSuccessor(NextBlock);
    //    ElseBodyBlock->AddSequentialSuccessor(NextBlock);
    //  }
    CurrentBlock->AddSequentialSuccessor(IfBodyBlock);
    CurrentBlock->AddConditionSuccessor(ElseBodyBlock);
  } else {
    /// If body successors are configured externally.
    //  if (NextBlock) {
    //    IfBodyBlock->AddSequentialSuccessor(NextBlock);
    //    ConditionBlock->AddConditionSuccessors(IfBodyBlock, NextBlock);
    //  }
  }
}

void CFGBuilder::Visit(std::unique_ptr<ASTIntegerLiteral> &&Integer) {
  CurrentBlock->AddStatement(std::move(Integer));
}

void CFGBuilder::Visit(std::unique_ptr<ASTReturnStmt> &&) {
  // Always means the end of control flow.
}

void CFGBuilder::Visit(std::unique_ptr<ASTStringLiteral> &&String) {
  CurrentBlock->AddStatement(std::move(String));
}

void CFGBuilder::Visit(std::unique_ptr<ASTSymbol> &&Symbol) {
  CurrentBlock->AddStatement(std::move(Symbol));
}

void CFGBuilder::Visit(std::unique_ptr<ASTUnaryOperator> &&Unary) {
  CurrentBlock->AddStatement(std::move(Unary));
}

void CFGBuilder::Visit(std::unique_ptr<ASTVarDecl> &&VarDecl) {
  CurrentBlock->AddStatement(std::move(VarDecl));
}

/*  while (cond) { body(); }
 *
 *               +--------+
 *               |  cond  |<-----
 *               +--------+      \
 *                 /    \        |
 *             no /      \ yes   |
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
void CFGBuilder::Visit(std::unique_ptr<ASTWhileStmt> &&While) {
  auto BodyBlock = std::make_shared<CFGBlock>();

  CurrentBlock->AddStatement(While->GetCondition());
  //  if (NextBlock)
  //    CurrentBlock->AddConditionSuccessors(BodyBlock, NextBlock);
  /// This should be handled recursively...
  BodyBlock->AddStatement(While->GetBody());
  BodyBlock->AddSequentialSuccessor(CurrentBlock);

  /// All other blocks expect the next after whole statement are linked with
  /// this.
  //  CurrentBlock = std::move(ConditionBlock);
}

} // namespace frontEnd
} // namespace weak