#include "FrontEnd/Analysis/CFGBlock.hpp"
#include "Utility/Diagnostic.hpp"
#include <algorithm>

namespace weak {
namespace frontEnd {

void CFGBlock::SetStmtType(Stmt Type) { StmtType = Type; }

CFGBlock::Stmt CFGBlock::GetStmtType() const { return StmtType; }

void CFGBlock::AddStatement(std::unique_ptr<ASTNode> &&Statement) {
  /// Guaranteed unique object.
  Statements.push_back(std::move(Statement));
}

void CFGBlock::AddSequentialSuccessor(const std::shared_ptr<CFGBlock> &Block) {
  Successors.first = Block;
  Block->Predecessors.push_back(shared_from_this());
}

void CFGBlock::AddConditionSuccessor(const std::shared_ptr<CFGBlock> &Block) {
  Successors.second = Block;
  Block->Predecessors.push_back(shared_from_this());
}

void CFGBlock::AddPredecessor(const std::shared_ptr<CFGBlock> &Block) {
  if (std::find(Predecessors.begin(), Predecessors.end(), Block) !=
      Predecessors.end()) {
    DiagnosticError(0U, 0U) << "CFG node is already linked as predecessor.";
    UnreachablePoint();
  }

  Predecessors.push_back(Block);
  Block->Successors.first = shared_from_this();
}

bool CFGBlock::IsPredecessorOf(const std::shared_ptr<CFGBlock> &Block) {
  return Successors.first == Block || Successors.second == Block;
}

bool CFGBlock::IsSuccessorOf(const std::shared_ptr<CFGBlock> &Block) {
  return Block->Successors.first == shared_from_this() ||
         Block->Successors.second == shared_from_this();
}

std::vector<std::unique_ptr<ASTNode>> &&CFGBlock::GetStatements() {
  return std::move(Statements);
}

const std::vector<std::unique_ptr<ASTNode>> &CFGBlock::GetStatements() const {
  return Statements;
}

const std::shared_ptr<CFGBlock> &CFGBlock::GetSequentialSuccessor() const {
  return Successors.first;
}

const std::shared_ptr<CFGBlock> &CFGBlock::GetConditionalSuccessor() const {
  return Successors.second;
}

const std::vector<std::shared_ptr<CFGBlock>> &
CFGBlock::GetPredecessors() const {
  return Predecessors;
}

} // namespace frontEnd
} // namespace weak