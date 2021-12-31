#include "FrontEnd/AST/ASTForStmt.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"

namespace weak {
namespace frontEnd {

ASTForStmt::ASTForStmt(std::unique_ptr<ASTNode> &&TheInit,
                       std::unique_ptr<ASTNode> &&TheCondition,
                       std::unique_ptr<ASTNode> &&TheIncrement,
                       std::unique_ptr<ASTCompoundStmt> &&TheBody,
                       unsigned TheLineNo, unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo), Init(std::move(TheInit)),
      Condition(std::move(TheCondition)), Increment(std::move(TheIncrement)),
      Body(std::move(TheBody)) {}

ASTType ASTForStmt::GetASTType() const { return ASTType::FOR_STMT; }

void ASTForStmt::Accept(const std::unique_ptr<ASTVisitor> &Visitor) const {
  Visitor->Visit(this);
}

const std::unique_ptr<ASTNode> &ASTForStmt::GetInit() const { return Init; }

const std::unique_ptr<ASTNode> &ASTForStmt::GetCondition() const {
  return Condition;
}

const std::unique_ptr<ASTNode> &ASTForStmt::GetIncrement() const {
  return Increment;
}

const std::unique_ptr<ASTCompoundStmt> &ASTForStmt::GetBody() const {
  return Body;
}

} // namespace frontEnd
} // namespace weak