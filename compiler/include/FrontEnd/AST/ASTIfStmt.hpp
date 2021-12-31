#ifndef WEAK_COMPILER_FRONTEND_AST_AST_IF_STMT_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_IF_STMT_HPP

#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include "FrontEnd/AST/ASTNode.hpp"

namespace weak {
namespace frontEnd {

class ASTIfStmt : public ASTNode {
public:
  ASTIfStmt(std::unique_ptr<ASTNode> &&TheCondition,
            std::unique_ptr<ASTCompoundStmt> &&TheThenBody,
            std::unique_ptr<ASTCompoundStmt> &&TheElseBody,
            unsigned TheLineNo = 0U, unsigned TheColumnNo = 0U);

  ASTType GetASTType() const override;
  void Accept(const std::unique_ptr<ASTVisitor> &) const override;

  const std::unique_ptr<ASTNode> &GetCondition() const;
  const std::unique_ptr<ASTCompoundStmt> &GetThenBody() const;
  const std::unique_ptr<ASTCompoundStmt> &GetElseBody() const;

private:
  std::unique_ptr<ASTNode> Condition;
  std::unique_ptr<ASTCompoundStmt> ThenBody;
  std::unique_ptr<ASTCompoundStmt> ElseBody;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_IF_STMT_HPP
