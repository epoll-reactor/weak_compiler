#ifndef WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_BLOCK_HPP
#define WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_BLOCK_HPP

#include "FrontEnd/AST/ASTNode.hpp"
#include "Utility/Uncopyable.hpp"
#include "Utility/Unmovable.hpp"
#include <vector>

namespace weak {
namespace frontEnd {
/*!
 * This is an implementation of Control Flow Graph node, which
 * stores exactly one copy of linked predecessors/successors.
 *
 * This class holds ownership of given AST statements.
 */
class CFGBlock : public std::enable_shared_from_this<CFGBlock>,
                 public Uncopyable {
public:
  void Link(const std::shared_ptr<CFGBlock> &With);
  void Unlink(const std::shared_ptr<CFGBlock> &With);

  bool LinkedWith(const std::shared_ptr<CFGBlock> &Block);

  void AddStatement(std::unique_ptr<ASTNode> &&Statement);
  void AddPredecessor(const std::shared_ptr<CFGBlock> &Block);
  void AddSuccessor(const std::shared_ptr<CFGBlock> &Block);

  const std::vector<std::unique_ptr<ASTNode>> &GetStatements() const;
  const std::vector<std::shared_ptr<CFGBlock>> &GetSuccessors() const;
  const std::vector<std::shared_ptr<CFGBlock>> &GetPredecessors() const;

private:
  std::vector<std::unique_ptr<ASTNode>> Statements;
  std::vector<std::shared_ptr<CFGBlock>> Successors, Predecessors;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_BLOCK_HPP