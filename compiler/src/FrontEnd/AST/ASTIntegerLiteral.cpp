#include "FrontEnd/AST/ASTIntegerLiteral.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTIntegerLiteral::ASTIntegerLiteral(signed TheValue, unsigned TheLineNo,
                                     unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Value(TheValue) {}

ASTType ASTIntegerLiteral::GetASTType() const {
  return ASTType::INTEGER_LITERAL;
}

void ASTIntegerLiteral::Accept(
    const std::unique_ptr<ASTVisitor> &Visitor) const {
  Visitor->Visit(this);
}

signed ASTIntegerLiteral::GetValue() const { return Value; }

} // namespace frontEnd
} // namespace weak