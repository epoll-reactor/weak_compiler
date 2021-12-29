#ifndef WEAK_COMPILER_FRONTEND_AST_AST_STRING_LITERAL_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_STRING_LITERAL_HPP

#include "FrontEnd/AST/ASTNode.hpp"
#include <string>

namespace weak {
namespace frontEnd {

class ASTStringLiteral : public ASTNode {
public:
  ASTStringLiteral(std::string TheValue, unsigned TheLineNo = 0U,
                   unsigned TheColumnNo = 0U);

  ASTType GetASTType() const override;

  const std::string &GetValue() const;

private:
  std::string Value;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_STRING_LITERAL_HPP
