#include "FrontEnd/AST/ASTVarDecl.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTVarDecl::ASTVarDecl(TokenType TheDataType, std::string &&TheSymbolName,
                       std::unique_ptr<ASTNode> &&TheDeclareBody,
                       unsigned TheLineNo, unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), DataType(TheDataType),
      SymbolName(std::move(TheSymbolName)),
      DeclareBody(std::move(TheDeclareBody)) {}

ASTType ASTVarDecl::GetASTType() const { return ASTType::VAR_DECL; }

void ASTVarDecl::Accept(const std::unique_ptr<ASTVisitor> &Visitor) const {
  Visitor->Visit(this);
}

TokenType ASTVarDecl::GetDataType() const { return DataType; }

const std::string &ASTVarDecl::GetSymbolName() const { return SymbolName; }

const std::unique_ptr<ASTNode> &ASTVarDecl::GetDeclareBody() const {
  return DeclareBody;
}

} // namespace frontEnd
} // namespace weak