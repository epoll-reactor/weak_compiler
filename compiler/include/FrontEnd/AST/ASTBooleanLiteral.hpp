#ifndef WEAK_COMPILER_FRONTEND_AST_AST_BOOLEAN_LITERAL_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_BOOLEAN_LITERAL_HPP

#include "FrontEnd/AST/ASTNode.hpp"

namespace weak {
namespace frontEnd {

class ASTBooleanLiteral : public ASTNode {
public:
  ASTBooleanLiteral(bool TheValue, unsigned TheLineNo = 0U,
                    unsigned TheColumnNo = 0U);

  ASTType GetASTType() const override;
  void Accept(const std::unique_ptr<ASTVisitor> &) const override;

  bool GetValue() const;

private:
  bool Value;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_BOOLEAN_LITERAL_HPP
