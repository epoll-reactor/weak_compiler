#include "FrontEnd/AST/ASTDoWhileStmt.hpp"

namespace weak {
namespace frontEnd {

weak::frontEnd::ASTDoWhileStmt::ASTDoWhileStmt(
    std::unique_ptr<ASTCompoundStmt> &&TheBody,
    std::unique_ptr<ASTNode> &&TheCondition, unsigned TheLineNo,
    unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Body(std::move(TheBody)),
      Condition(std::move(TheCondition)) {}

ASTType ASTDoWhileStmt::GetASTType() const { return ASTType::DO_WHILE_STMT; }

const std::unique_ptr<ASTCompoundStmt> &ASTDoWhileStmt::GetBody() const {
  return Body;
}
const std::unique_ptr<ASTNode> &ASTDoWhileStmt::GetCondition() const {
  return Condition;
}

} // namespace frontEnd
} // namespace weak