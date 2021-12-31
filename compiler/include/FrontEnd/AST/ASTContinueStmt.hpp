#ifndef WEAK_COMPILER_FRONTEND_AST_AST_CONTINUE_STMT_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_CONTINUE_STMT_HPP

#include "FrontEnd/AST/ASTNode.hpp"

namespace weak {
namespace frontEnd {

class ASTContinueStmt : public ASTNode {
public:
  ASTContinueStmt(unsigned TheLineNo = 0U, unsigned TheColumnNo = 0U);

  ASTType GetASTType() const override;
  void Accept(const std::unique_ptr<ASTVisitor> &) const override;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_CONTINUE_STMT_HPP