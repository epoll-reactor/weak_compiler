#include "FrontEnd/AST/ASTContinueStmt.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTContinueStmt::ASTContinueStmt(unsigned TheLineNo, unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo) {}

ASTType ASTContinueStmt::GetASTType() const { return ASTType::CONTINUE_STMT; }

void ASTContinueStmt::Accept(const std::unique_ptr<ASTVisitor> &Visitor) const {
  Visitor->Visit(this);
}

} // namespace frontEnd
} // namespace weak