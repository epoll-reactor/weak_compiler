#ifndef WEAK_COMPILER_FRONTEND_AST_AST_NODE_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_NODE_HPP

#include "FrontEnd/AST/ASTTypesEnum.hpp"

namespace weak {
namespace frontEnd {

class ASTNode {
public:
  virtual ~ASTNode() noexcept = default;
  virtual ASTType GetASTType() const { return ASTType::BASE_NODE; }

  unsigned GetLineNo() const { return LineNo; }
  unsigned GetColumnNo() const { return ColumnNo; }

protected:
  ASTNode(unsigned TheLineNo, unsigned TheColumnNo)
      : LineNo(TheLineNo), ColumnNo(TheColumnNo) {}

  unsigned LineNo;
  unsigned ColumnNo;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_NODE_HPP
