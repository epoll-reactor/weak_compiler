#ifndef WEAK_COMPILER_FRONTEND_AST_AST_NODE_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_NODE_HPP

#include "FrontEnd/AST/ASTTypesEnum.hpp"

namespace weak {
namespace frontEnd {

class ASTNode {
public:
  virtual ~ASTNode() noexcept = default;
  virtual ASTType GetASTType() const;

  unsigned GetLineNo() const;
  unsigned GetColumnNo() const;

protected:
  ASTNode(unsigned TheLineNo, unsigned TheColumnNo);

  unsigned LineNo;
  unsigned ColumnNo;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_NODE_HPP
