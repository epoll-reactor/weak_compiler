/* ASTNode.hpp - Basic AST node.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_FRONTEND_AST_AST_NODE_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_NODE_HPP

#include "FrontEnd/AST/ASTTypesEnum.hpp"
#include <memory>

namespace weak {
namespace frontEnd {

class ASTVisitor;

class ASTNode {
public:
  virtual ~ASTNode() noexcept = default;
  virtual ASTType GetASTType() const;

  unsigned GetLineNo() const;
  unsigned GetColumnNo() const;

protected:
  ASTNode(unsigned TheLineNo, unsigned TheColumnNo);

  unsigned LineNo;
  unsigned ColumnNo;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_NODE_HPP
