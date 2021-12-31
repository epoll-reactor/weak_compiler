#include "FrontEnd/AST/ASTStringLiteral.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTStringLiteral::ASTStringLiteral(std::string TheValue, unsigned TheLineNo,
                                   unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Value(std::move(TheValue)) {}

ASTType ASTStringLiteral::GetASTType() const { return ASTType::STRING_LITERAL; }

void ASTStringLiteral::Accept(
    const std::unique_ptr<ASTVisitor> &Visitor) const {
  Visitor->Visit(this);
}

const std::string &ASTStringLiteral::GetValue() const { return Value; }

} // namespace frontEnd
} // namespace weak