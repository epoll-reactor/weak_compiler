/* ASTWhileStmt.hpp - AST node to represent a while statement.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_FRONTEND_AST_AST_WHILE_STMT_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_WHILE_STMT_HPP

#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include "FrontEnd/AST/ASTNode.hpp"

namespace weak {
namespace frontEnd {

class ASTWhileStmt : public ASTNode {
public:
  ASTWhileStmt(std::unique_ptr<ASTNode> &&TheCondition,
               std::unique_ptr<ASTCompoundStmt> &&TheBody,
               unsigned TheLineNo = 0U, unsigned TheColumnNo = 0U);

  ASTType GetASTType() const override;
  void Accept(const ASTVisitor *) const override;

  std::unique_ptr<ASTNode> &&GetCondition();
  const std::unique_ptr<ASTNode> &GetCondition() const;
  std::unique_ptr<ASTCompoundStmt> &&GetBody();
  const std::unique_ptr<ASTCompoundStmt> &GetBody() const;

private:
  std::unique_ptr<ASTNode> Condition;
  std::unique_ptr<ASTCompoundStmt> Body;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_WHILE_STMT_HPP
