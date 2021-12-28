#include "FrontEnd/AST/ASTBooleanLiteral.hpp"

namespace weak {
namespace frontEnd {

ASTBooleanLiteral::ASTBooleanLiteral(bool TheValue, unsigned TheLineNo,
                                     unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Value(TheValue) {}

ASTType ASTBooleanLiteral::GetASTType() const {
  return ASTType::BOOLEAN_LITERAL;
}

bool ASTBooleanLiteral::GetValue() const { return Value; }

} // namespace frontEnd
} // namespace weak