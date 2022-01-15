/* ASTUnaryOperator.hpp - AST node to represent a unary operator.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_FRONTEND_AST_AST_UNARY_OPERATOR_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_UNARY_OPERATOR_HPP

#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/Lex/Token.hpp"
#include <memory>

namespace weak {
namespace frontEnd {

class ASTUnaryOperator : public ASTNode {
public:
  enum struct UnaryType { PREFIX, POSTFIX } const PrefixOrPostfix;

  ASTUnaryOperator(UnaryType ThePrefixOrPostfix, TokenType TheOperation,
                   std::unique_ptr<ASTNode> &&TheOperand,
                   unsigned TheLineNo = 0U, unsigned TheColumnNo = 0U);

  ASTType GetASTType() const override;
  void Accept(const ASTVisitor *) const override;

  TokenType GetOperation() const;
  const std::unique_ptr<ASTNode> &GetOperand() const;

private:
  TokenType Operation;
  std::unique_ptr<ASTNode> Operand;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_UNARY_OPERATOR_HPP
