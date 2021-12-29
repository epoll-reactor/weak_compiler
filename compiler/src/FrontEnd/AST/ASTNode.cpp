#include "FrontEnd/AST/ASTNode.hpp"

weak::frontEnd::ASTNode::ASTNode(unsigned TheLineNo, unsigned TheColumnNo)
    : LineNo(TheLineNo), ColumnNo(TheColumnNo) {}

weak::frontEnd::ASTType weak::frontEnd::ASTNode::GetASTType() const {
  return ASTType::BASE_NODE;
}

unsigned weak::frontEnd::ASTNode::GetLineNo() const { return LineNo; }

unsigned weak::frontEnd::ASTNode::GetColumnNo() const { return ColumnNo; }
