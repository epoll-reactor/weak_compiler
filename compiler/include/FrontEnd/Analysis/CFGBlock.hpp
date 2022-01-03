#ifndef WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_BLOCK_HPP
#define WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_BLOCK_HPP

#include "FrontEnd/AST/ASTNode.hpp"
#include "Utility/Uncopyable.hpp"
#include "Utility/Unmovable.hpp"
#include <vector>

namespace weak {
namespace frontEnd {
/*! This is an implementation of Control Flow Graph node. */
class CFGBlock : public Uncopyable, public Unmovable {
public:
  CFGBlock(std::vector<std::unique_ptr<ASTNode>> &&TheStatements);

  void Link(CFGBlock &With);
  void Unlink(CFGBlock &With);

  bool LinkedWith(CFGBlock &With);

  void AddPredecessor(CFGBlock &Block);
  void AddSuccessor(CFGBlock &Block);

  const std::vector<std::unique_ptr<ASTNode>> &GetStatements() const;
  const std::vector<CFGBlock *> &GetSuccessors() const;
  const std::vector<CFGBlock *> &GetPredecessors() const;

private:
  std::vector<std::unique_ptr<ASTNode>> Statements;
  std::vector<CFGBlock *> Successors, Predecessors;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_BLOCK_HPP