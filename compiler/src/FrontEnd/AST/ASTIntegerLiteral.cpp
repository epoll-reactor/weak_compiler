#include "FrontEnd/AST/ASTIntegerLiteral.hpp"

namespace weak {
namespace frontEnd {

ASTIntegerLiteral::ASTIntegerLiteral(signed TheValue, unsigned TheLineNo,
                                     unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Value(TheValue) {}

ASTType ASTIntegerLiteral::GetASTType() const {
  return ASTType::INTEGER_LITERAL;
}

signed ASTIntegerLiteral::GetValue() const { return Value; }

} // namespace frontEnd
} // namespace weak