#include "FrontEnd/AST/ASTIfStmt.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTIfStmt::ASTIfStmt(std::unique_ptr<ASTNode> &&TheCondition,
                     std::unique_ptr<ASTCompoundStmt> &&TheThenBody,
                     std::unique_ptr<ASTCompoundStmt> &&TheElseBody,
                     unsigned TheLineNo, unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Condition(std::move(TheCondition)),
      ThenBody(std::move(TheThenBody)), ElseBody(std::move(TheElseBody)) {}

ASTType ASTIfStmt::GetASTType() const { return ASTType::IF_STMT; }

void ASTIfStmt::Accept(const std::unique_ptr<ASTVisitor> &Visitor) const {
  Visitor->Visit(this);
}

const std::unique_ptr<ASTNode> &ASTIfStmt::GetCondition() const {
  return Condition;
}

const std::unique_ptr<ASTCompoundStmt> &ASTIfStmt::GetThenBody() const {
  return ThenBody;
}

const std::unique_ptr<ASTCompoundStmt> &ASTIfStmt::GetElseBody() const {
  return ElseBody;
}

} // namespace frontEnd
} // namespace weak