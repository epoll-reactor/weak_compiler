/* ASTReturnStmt.hpp - AST node to represent a return statement.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_FRONTEND_AST_AST_RETURN_STMT_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_RETURN_STMT_HPP

#include "FrontEnd/AST/ASTNode.hpp"
#include <memory>

namespace weak {
namespace frontEnd {

class ASTReturnStmt : public ASTNode {
public:
  ASTReturnStmt(std::unique_ptr<ASTNode> &&TheOperand, unsigned TheLineNo = 0U,
                unsigned TheColumnNo = 0U);

  ASTType GetASTType() const override;

  const std::unique_ptr<ASTNode> &GetOperand() const;

private:
  std::unique_ptr<ASTNode> Operand;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_RETURN_STMT_HPP
