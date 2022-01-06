#include "FrontEnd/Analysis/CFGBlock.hpp"
#include "Utility/Diagnostic.hpp"
#include <algorithm>

namespace weak {
namespace frontEnd {

void CFGBlock::Link(const std::shared_ptr<CFGBlock> &With) {
  if (std::find(Successors.begin(), Successors.end(), With) != Successors.end())
    DiagnosticError(0U, 0U) << "Given CFG block is already linked.";

  AddSuccessor(std::move(With));
  With->AddPredecessor(shared_from_this());
}

void CFGBlock::Unlink(const std::shared_ptr<CFGBlock> &With) {
  auto FoundedBlock = std::find(Successors.begin(), Successors.end(), With);
  if (FoundedBlock == Successors.end()) {
    DiagnosticError(0U, 0U) << "Attempt to unlink non-existent CFG block.";
    UnreachablePoint();
  }

  Successors.erase(FoundedBlock);
  With->Predecessors.erase(
      std::remove_if(With->Predecessors.begin(), With->Predecessors.end(),
                     [this](const std::shared_ptr<CFGBlock> &BlockPtr) {
                       return this == BlockPtr.get();
                     }),
      With->Predecessors.end());
}

bool CFGBlock::LinkedWith(const std::shared_ptr<CFGBlock> &Block) {
  auto ThisLinked = std::find(Successors.begin(), Successors.end(), Block);
  auto OtherLinked = std::find(Block->Predecessors.begin(),
                               Block->Predecessors.end(), shared_from_this());

  return (ThisLinked != Successors.end()) &&
         (OtherLinked != Block->Predecessors.end());
}

void CFGBlock::AddStatement(std::unique_ptr<ASTNode> &&Statement) {
  /// Guaranteed unique object.
  Statements.push_back(std::move(Statement));
}

void CFGBlock::AddSuccessor(const std::shared_ptr<CFGBlock> &Block) {
  if (std::find(Successors.begin(), Successors.end(), Block) ==
      Successors.end()) {
    Successors.push_back(std::move(Block));
  } else {
    DiagnosticError(0U, 0U) << "Given successor CFG is already linked.";
    UnreachablePoint();
  }
}

void CFGBlock::AddPredecessor(const std::shared_ptr<CFGBlock> &Block) {
  if (std::find(Predecessors.begin(), Predecessors.end(), Block) ==
      Predecessors.end()) {
    Predecessors.push_back(std::move(Block));
  } else {
    DiagnosticError(0U, 0U) << "Given predecessor CFG is already linked.";
    UnreachablePoint();
  }
}

const std::vector<std::unique_ptr<ASTNode>> &CFGBlock::GetStatements() const {
  return Statements;
}

const std::vector<std::shared_ptr<CFGBlock>> &CFGBlock::GetSuccessors() const {
  return Successors;
}

const std::vector<std::shared_ptr<CFGBlock>> &
CFGBlock::GetPredecessors() const {
  return Predecessors;
}

} // namespace frontEnd
} // namespace weak