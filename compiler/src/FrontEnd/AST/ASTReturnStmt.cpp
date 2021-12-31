#include "FrontEnd/AST/ASTReturnStmt.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTReturnStmt::ASTReturnStmt(std::unique_ptr<ASTNode> &&TheOperand,
                             unsigned TheLineNo, unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Operand(std::move(TheOperand)) {}

ASTType ASTReturnStmt::GetASTType() const { return ASTType::RETURN_STMT; }

const std::unique_ptr<ASTNode> &ASTReturnStmt::GetOperand() const {
  return Operand;
}

} // namespace frontEnd
} // namespace weak