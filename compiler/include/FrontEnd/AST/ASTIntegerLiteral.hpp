/* ASTIntegerLiteral.hpp - AST node to represent a integer number.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_FRONTEND_AST_AST_INTEGER_LITERAL_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_INTEGER_LITERAL_HPP

#include "FrontEnd/AST/ASTNode.hpp"

namespace weak {
namespace frontEnd {

class ASTIntegerLiteral : public ASTNode {
public:
  ASTIntegerLiteral(signed TheValue, unsigned TheLineNo = 0U,
                    unsigned TheColumnNo = 0U);

  ASTType GetASTType() const override;
  void Accept(const ASTVisitor *) const override;

  signed GetValue() const;

private:
  signed Value;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_INTEGER_LITERAL_HPP
