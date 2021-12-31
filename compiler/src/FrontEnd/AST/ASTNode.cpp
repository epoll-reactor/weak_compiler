#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTNode::ASTNode(unsigned TheLineNo, unsigned TheColumnNo)
    : LineNo(TheLineNo), ColumnNo(TheColumnNo) {}

ASTType ASTNode::GetASTType() const { return ASTType::BASE_NODE; }

unsigned ASTNode::GetLineNo() const { return LineNo; }

unsigned ASTNode::GetColumnNo() const { return ColumnNo; }

} // namespace frontEnd
} // namespace weak