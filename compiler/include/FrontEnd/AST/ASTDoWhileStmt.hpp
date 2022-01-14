/* ASTDoWhileStmt.hpp - AST node to represent a do-while statement.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_FRONTEND_AST_AST_DO_WHILE_STMT_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_DO_WHILE_STMT_HPP

#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include "FrontEnd/AST/ASTNode.hpp"

namespace weak {
namespace frontEnd {

class ASTDoWhileStmt : public ASTNode {
public:
  ASTDoWhileStmt(std::unique_ptr<ASTCompoundStmt> &&TheBody,
                 std::unique_ptr<ASTNode> &&TheCondition,
                 unsigned TheLineNo = 0U, unsigned TheColumnNo = 0U);

  ASTType GetASTType() const override;

  std::unique_ptr<ASTCompoundStmt> &&GetBody();
  const std::unique_ptr<ASTCompoundStmt> &GetBody() const;
  std::unique_ptr<ASTNode> &&GetCondition();
  const std::unique_ptr<ASTNode> &GetCondition() const;

private:
  std::unique_ptr<ASTCompoundStmt> Body;
  std::unique_ptr<ASTNode> Condition;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_DO_WHILE_STMT_HPP
