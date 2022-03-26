#include "MiddleEnd/IR/VariableSearchVisitor.hpp"
#include "FrontEnd/AST/ASTBinaryOperator.hpp"
#include "MiddleEnd/IR/IRAssignment.hpp"
#include "MiddleEnd/IR/IRBranch.hpp"

using namespace weak::frontEnd;

namespace weak {
namespace middleEnd {

std::set<ASTSymbol *>
VariableSearchVisitor::AllVarsUsedInStatement(IRNode *Stmt) {
  Variables.clear();
  Stmt->Accept(this);
  return Variables;
}

void VariableSearchVisitor::Visit(const IRBranch *Stmt) const {
  if (Stmt->Condition)
    Stmt->Condition->Accept(this);
}

void VariableSearchVisitor::Visit(const IRAssignment *Stmt) const {
  Stmt->GetOperand()->Accept(this);
}

void VariableSearchVisitor::Visit(const ASTSymbol *Stmt) const {
  Variables.insert(const_cast<ASTSymbol *>(Stmt));
}

void VariableSearchVisitor::Visit(const ASTBinaryOperator *Stmt) const {
  if (Stmt->GetOperation() == TokenType::ASSIGN) {
    Variables.insert(static_cast<ASTSymbol *>(Stmt->GetLHS().get()));
  }
  Stmt->GetLHS()->Accept(this);
  Stmt->GetRHS()->Accept(this);
}

} // namespace middleEnd
} // namespace weak