/* ASTFloatingPointLiteral.hpp - AST node to represent a floating point number.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_FRONTEND_AST_AST_FLOATING_POINT_LITERAL_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_FLOATING_POINT_LITERAL_HPP

#include "FrontEnd/AST/ASTNode.hpp"

namespace weak {
namespace frontEnd {

class ASTFloatingPointLiteral : public ASTNode {
public:
  ASTFloatingPointLiteral(double TheValue, unsigned TheLineNo = 0U,
                          unsigned TheColumnNo = 0U);

  ASTType GetASTType() const override;

  double GetValue() const;

private:
  double Value;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_FLOATING_POINT_LITERAL_HPP
