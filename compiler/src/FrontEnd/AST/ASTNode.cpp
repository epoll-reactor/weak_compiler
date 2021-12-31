#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTNode::ASTNode(unsigned TheLineNo, unsigned TheColumnNo)
    : LineNo(TheLineNo), ColumnNo(TheColumnNo) {}

ASTType ASTNode::GetASTType() const { return ASTType::BASE_NODE; }

void ASTNode::Accept(const std::unique_ptr<ASTVisitor> &Visitor) const {
  Visitor->Visit(this);
}

unsigned ASTNode::GetLineNo() const { return LineNo; }

unsigned ASTNode::GetColumnNo() const { return ColumnNo; }

} // namespace frontEnd
} // namespace weak