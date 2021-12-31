#include "FrontEnd/AST/ASTFloatingPointLiteral.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTFloatingPointLiteral::ASTFloatingPointLiteral(double TheValue,
                                                 unsigned TheLineNo,
                                                 unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Value(TheValue) {}

ASTType ASTFloatingPointLiteral::GetASTType() const {
  return ASTType::FLOATING_POINT_LITERAL;
}

void ASTFloatingPointLiteral::Accept(
    const std::unique_ptr<ASTVisitor> &Visitor) const {
  Visitor->Visit(this);
}

double ASTFloatingPointLiteral::GetValue() const { return Value; }

} // namespace frontEnd
} // namespace weak