#ifndef WEAK_COMPILER_FRONTEND_AST_AST_VISITOR_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_VISITOR_HPP

namespace weak {
namespace frontEnd {

class ASTVisitor {
public:
  virtual ~ASTVisitor() noexcept = default;
  /// Visit all types of nodes.
  void VisitBaseNode(const class ASTNode *) const;
  virtual void Visit(const class ASTBinaryOperator *) const = 0;
  virtual void Visit(const class ASTBooleanLiteral *) const = 0;
  virtual void Visit(const class ASTBreakStmt *) const = 0;
  virtual void Visit(const class ASTCompoundStmt *) const = 0;
  virtual void Visit(const class ASTContinueStmt *) const = 0;
  virtual void Visit(const class ASTDoWhileStmt *) const = 0;
  virtual void Visit(const class ASTFloatingPointLiteral *) const = 0;
  virtual void Visit(const class ASTForStmt *) const = 0;
  virtual void Visit(const class ASTFunctionDecl *) const = 0;
  virtual void Visit(const class ASTIfStmt *) const = 0;
  virtual void Visit(const class ASTIntegerLiteral *) const = 0;
  virtual void Visit(const class ASTReturnStmt *) const = 0;
  virtual void Visit(const class ASTStringLiteral *) const = 0;
  virtual void Visit(const class ASTSymbol *) const = 0;
  virtual void Visit(const class ASTUnaryOperator *) const = 0;
  virtual void Visit(const class ASTVarDecl *) const = 0;
  virtual void Visit(const class ASTWhileStmt *) const = 0;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_VISITOR_HPP
