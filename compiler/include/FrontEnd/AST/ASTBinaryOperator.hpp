#ifndef WEAK_COMPILER_FRONTEND_AST_AST_BINARY_OPERATOR_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_BINARY_OPERATOR_HPP

#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/Lex/Token.hpp"
#include <memory>

namespace weak {
namespace frontEnd {

class ASTBinaryOperator : public ASTNode {
public:
  ASTBinaryOperator(TokenType TheOperation, std::unique_ptr<ASTNode> &&TheLHS,
                    std::unique_ptr<ASTNode> &&TheRHS, unsigned TheLineNo = 0U,
                    unsigned TheColumnNo = 0U);

  ASTType GetASTType() const override;
  void Accept(const std::unique_ptr<ASTVisitor> &) const override;

  TokenType GetOperation() const;
  const std::unique_ptr<ASTNode> &GetLHS() const;
  const std::unique_ptr<ASTNode> &GetRHS() const;

private:
  TokenType Operation;
  std::unique_ptr<ASTNode> LHS;
  std::unique_ptr<ASTNode> RHS;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_BINARY_OPERATOR_HPP
