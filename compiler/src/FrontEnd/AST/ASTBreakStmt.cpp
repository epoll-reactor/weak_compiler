#include "FrontEnd/AST/ASTBreakStmt.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTBreakStmt::ASTBreakStmt(unsigned TheLineNo, unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo) {}

ASTType ASTBreakStmt::GetASTType() const { return ASTType::BREAK_STMT; }

void ASTBreakStmt::Accept(const std::unique_ptr<ASTVisitor> &Visitor) const {
  Visitor->Visit(this);
}

} // namespace frontEnd
} // namespace weak