#ifndef WEAK_COMPILER_FRONTEND_AST_AST_INTEGER_LITERAL_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_INTEGER_LITERAL_HPP

#include "FrontEnd/AST/ASTNode.hpp"

namespace weak {
namespace frontEnd {

class ASTIntegerLiteral : public ASTNode {
public:
  ASTIntegerLiteral(signed TheValue, unsigned TheLineNo = 0,
                    unsigned TheColumnNo = 0);

  ASTType GetASTType() const override;

  signed GetValue() const;

private:
  signed Value;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_INTEGER_LITERAL_HPP
