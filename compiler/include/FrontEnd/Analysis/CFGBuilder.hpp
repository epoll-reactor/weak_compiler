#ifndef WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_BUILDER_HPP
#define WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_BUILDER_HPP

#include "FrontEnd/Analysis/CFGBlock.hpp"

namespace weak {
namespace frontEnd {

/*! This implements transformer from AST to CFG.
 *
 *  CFG retains ownership of given AST and can change it during building.
 *
 *  The CFG consists of blocks, each of which contains statements sequence
 *  without branching and ambiguous transitions, e.g loops. If we have some
 *  linked together CFG blocks, all exit paths should lead to the next
 *  statement or represent the end of graph (in general, the end of function).
 *
 *  E.g this code:
 *    ++first; f1();
 *    do {
 *      ++second;
 *      f2();
 *    } while (++third);
 *  transformed to:
 *
 *                    +----------+
 *                    |  ++first |
 *                    |     f1() |
 *                    +----------+
 *                         |
 *                    +----------+
 *                    | ++second |<----
 *                    |     f2() |     |
 *                    +----------+     |
 *                         |           |
 *                    +----------+     |
 *                    | ++third  |     |
 *                    +----------+     |
 *                      /     \        |
 *                 no  /       \  yes  |
 *              +-----------+   \_____/
 *              | Next stmt |
 *              |   or end. |
 *              +-----------+
 *
 *  The result of the whole work is an array of CFG's, each of which
 *  represents a function.
 */
class CFGBuilder {
public:
  CFGBuilder(std::unique_ptr<ASTNode> &&TheRootNode);

  std::vector<std::shared_ptr<CFGBlock>> BuildCFG();

private:
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTNode> &&);
  std::shared_ptr<CFGBlock>
  VisitBinary(std::unique_ptr<class ASTBinaryOperator> &&);
  std::shared_ptr<CFGBlock>
  VisitBoolean(std::unique_ptr<class ASTBooleanLiteral> &&);
  std::shared_ptr<CFGBlock> VisitBreak(std::unique_ptr<class ASTBreakStmt> &&);
  std::shared_ptr<CFGBlock>
  VisitCompound(std::unique_ptr<class ASTCompoundStmt> &&);
  std::shared_ptr<CFGBlock>
  VisitContinue(std::unique_ptr<class ASTContinueStmt> &&);
  std::shared_ptr<CFGBlock>
  VisitDoWhile(std::unique_ptr<class ASTDoWhileStmt> &&);
  std::shared_ptr<CFGBlock>
  VisitFloat(std::unique_ptr<class ASTFloatingPointLiteral> &&);
  std::shared_ptr<CFGBlock> VisitFor(std::unique_ptr<class ASTForStmt> &&);
  std::shared_ptr<CFGBlock>
  VisitFunctionCall(std::unique_ptr<class ASTFunctionCall> &&);
  std::shared_ptr<CFGBlock>
  VisitFunctionDecl(std::unique_ptr<class ASTFunctionDecl> &&);
  std::shared_ptr<CFGBlock> VisitIf(std::unique_ptr<class ASTIfStmt> &&);
  std::shared_ptr<CFGBlock>
  VisitInteger(std::unique_ptr<class ASTIntegerLiteral> &&);
  std::shared_ptr<CFGBlock>
  VisitReturn(std::unique_ptr<class ASTReturnStmt> &&);
  std::shared_ptr<CFGBlock>
  VisitString(std::unique_ptr<class ASTStringLiteral> &&);
  std::shared_ptr<CFGBlock> VisitSymbol(std::unique_ptr<class ASTSymbol> &&);
  std::shared_ptr<CFGBlock>
  VisitUnary(std::unique_ptr<class ASTUnaryOperator> &&);
  std::shared_ptr<CFGBlock> VisitVarDecl(std::unique_ptr<class ASTVarDecl> &&);
  std::shared_ptr<CFGBlock> VisitWhile(std::unique_ptr<class ASTWhileStmt> &&);

  std::shared_ptr<CFGBlock> CreateCFGBlock();

  //  std::shared_ptr<CFGBlock> CurrentBlock;

  std::unique_ptr<ASTNode> RootNode;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_BUILDER_HPP