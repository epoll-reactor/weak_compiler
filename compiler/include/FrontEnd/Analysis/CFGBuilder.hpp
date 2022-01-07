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
  void VisitBaseNode(std::unique_ptr<ASTNode> &&);
  void Visit(std::unique_ptr<class ASTBinaryOperator> &&);
  void Visit(std::unique_ptr<class ASTBooleanLiteral> &&);
  void Visit(std::unique_ptr<class ASTBreakStmt> &&);
  void Visit(std::unique_ptr<class ASTCompoundStmt> &&);
  void Visit(std::unique_ptr<class ASTContinueStmt> &&);
  void Visit(std::unique_ptr<class ASTDoWhileStmt> &&);
  void Visit(std::unique_ptr<class ASTFloatingPointLiteral> &&);
  void Visit(std::unique_ptr<class ASTForStmt> &&);
  void Visit(std::unique_ptr<class ASTFunctionDecl> &&);
  void Visit(std::unique_ptr<class ASTFunctionCall> &&);
  void Visit(std::unique_ptr<class ASTIfStmt> &&);
  void Visit(std::unique_ptr<class ASTIntegerLiteral> &&);
  void Visit(std::unique_ptr<class ASTReturnStmt> &&);
  void Visit(std::unique_ptr<class ASTStringLiteral> &&);
  void Visit(std::unique_ptr<class ASTSymbol> &&);
  void Visit(std::unique_ptr<class ASTUnaryOperator> &&);
  void Visit(std::unique_ptr<class ASTVarDecl> &&);
  void Visit(std::unique_ptr<class ASTWhileStmt> &&);

  /// Functions represented as CFG.
  std::vector<std::shared_ptr<CFGBlock>> FunctionGraphs;

  std::shared_ptr<CFGBlock> CurrentBlock;

  std::unique_ptr<ASTNode> RootNode;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_BUILDER_HPP