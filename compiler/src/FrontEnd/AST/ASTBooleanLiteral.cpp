#include "FrontEnd/AST/ASTBooleanLiteral.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTBooleanLiteral::ASTBooleanLiteral(bool TheValue, unsigned TheLineNo,
                                     unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Value(TheValue) {}

ASTType ASTBooleanLiteral::GetASTType() const {
  return ASTType::BOOLEAN_LITERAL;
}

void ASTBooleanLiteral::Accept(
    const std::unique_ptr<ASTVisitor> &Visitor) const {
  Visitor->Visit(this);
}

bool ASTBooleanLiteral::GetValue() const { return Value; }

} // namespace frontEnd
} // namespace weak