/* CFGBuilder.hpp - Transformer from AST to CFG.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_BUILDER_HPP
#define WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_BUILDER_HPP

#include "FrontEnd/Analysis/CFGBlock.hpp"

namespace weak {
namespace frontEnd {

class ASTNode;
class ASTBinaryOperator;
class ASTBooleanLiteral;
class ASTBreakStmt;
class ASTCompoundStmt;
class ASTContinueStmt;
class ASTDoWhileStmt;
class ASTFloatingPointLiteral;
class ASTForStmt;
class ASTFunctionDecl;
class ASTFunctionCall;
class ASTIfStmt;
class ASTIntegerLiteral;
class ASTReturnStmt;
class ASTStringLiteral;
class ASTSymbol;
class ASTUnaryOperator;
class ASTVarDecl;
class ASTWhileStmt;

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
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTBinaryOperator> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTBooleanLiteral> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTBreakStmt> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTCompoundStmt> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTContinueStmt> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTDoWhileStmt> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTFloatingPointLiteral> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTForStmt> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTFunctionCall> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTFunctionDecl> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTIfStmt> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTIntegerLiteral> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTReturnStmt> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTStringLiteral> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTSymbol> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTUnaryOperator> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTVarDecl> &&);
  std::shared_ptr<CFGBlock> Visit(std::unique_ptr<ASTWhileStmt> &&);

  std::shared_ptr<CFGBlock> CreateCFGBlock();

  //  std::shared_ptr<CFGBlock> CurrentBlock;

  std::unique_ptr<ASTNode> RootNode;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_BUILDER_HPP