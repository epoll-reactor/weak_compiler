#ifndef WEAK_COMPILER_FRONTEND_AST_AST_FUNCTION_DECL_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_FUNCTION_DECL_HPP

#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/Lex/Token.hpp"
#include <memory>
#include <string>
#include <vector>

namespace weak {
namespace frontEnd {

class ASTFunctionDecl : public ASTNode {
public:
  ASTFunctionDecl(TokenType TheReturnType, std::string &&TheName,
                  std::vector<std::unique_ptr<ASTNode>> &&TheArguments,
                  std::unique_ptr<ASTCompoundStmt> &&TheBody,
                  unsigned TheLineNo = 0U, unsigned TheColumnNo = 0U);

  ASTType GetASTType() const override;

  TokenType GetReturnType() const;
  const std::string &GetName() const;
  std::vector<std::unique_ptr<ASTNode>> &&GetArguments();
  const std::vector<std::unique_ptr<ASTNode>> &GetArguments() const;
  std::unique_ptr<ASTCompoundStmt> &&GetBody();
  const std::unique_ptr<ASTCompoundStmt> &GetBody() const;

private:
  TokenType ReturnType;
  std::string Name;
  std::vector<std::unique_ptr<ASTNode>> Arguments;
  std::unique_ptr<ASTCompoundStmt> Body;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_FUNCTION_DECL_HPP
