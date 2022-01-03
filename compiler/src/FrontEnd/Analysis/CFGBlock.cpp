#include "FrontEnd/Analysis/CFGBlock.hpp"
#include "Utility/Diagnostic.hpp"
#include <algorithm>

namespace weak {
namespace frontEnd {

CFGBlock::CFGBlock(std::vector<std::unique_ptr<ASTNode>> &&TheStatements)
    : Statements(std::move(TheStatements)) {}

void CFGBlock::Link(CFGBlock &With) {
  AddSuccessor(With);
  With.AddPredecessor(*this);
}

void CFGBlock::Unlink(CFGBlock &With) {
  auto FoundedBlock = std::find(Successors.begin(), Successors.end(), &With);
  if (FoundedBlock == Successors.end()) {
    DiagnosticError(0U, 0U) << "Attempt to unlink non-existent CFG block.";
    UnreachablePoint();
  }
  Successors.erase(FoundedBlock);
  With.Predecessors.erase(
      std::remove(With.Predecessors.begin(), With.Predecessors.end(), this),
      With.Predecessors.end());
}

bool CFGBlock::LinkedWith(CFGBlock &With) {
  auto ThisLinked = std::find(Successors.begin(), Successors.end(), &With);
  auto OtherLinked =
      std::find(With.Predecessors.begin(), With.Predecessors.end(), this);

  return (ThisLinked != Successors.end()) &&
         (OtherLinked != With.Predecessors.end());
}

void CFGBlock::AddSuccessor(CFGBlock &Block) { Successors.push_back(&Block); }

void CFGBlock::AddPredecessor(CFGBlock &Block) {
  Predecessors.push_back(&Block);
}

const std::vector<std::unique_ptr<ASTNode>> &CFGBlock::GetStatements() const {
  return Statements;
}

const std::vector<CFGBlock *> &CFGBlock::GetSuccessors() const {
  return Successors;
}

const std::vector<CFGBlock *> &CFGBlock::GetPredecessors() const {
  return Predecessors;
}

} // namespace frontEnd
} // namespace weak