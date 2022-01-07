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
  using SuccessorPair =
      std::pair<std::shared_ptr<CFGBlock>, std::shared_ptr<CFGBlock>>;

  void AddStatement(std::unique_ptr<ASTNode> &&Statement);
  void AddSequentialSuccessor(const std::shared_ptr<CFGBlock> &Block);
  void AddConditionSuccessor(const std::shared_ptr<CFGBlock> &Block);
  void AddPredecessor(const std::shared_ptr<CFGBlock> &Block);

  bool IsSuccessorOf(const std::shared_ptr<CFGBlock> &Block);
  bool IsPredecessorOf(const std::shared_ptr<CFGBlock> &Block);

  std::vector<std::unique_ptr<ASTNode>> &&GetStatements();
  const std::vector<std::unique_ptr<ASTNode>> &GetStatements() const;
  const std::shared_ptr<CFGBlock> &GetSequentialSuccessor() const;
  const std::shared_ptr<CFGBlock> &GetConditionalSuccessor() const;
  const std::vector<std::shared_ptr<CFGBlock>> &GetPredecessors() const;

private:
  /// Stored inside statements.
  std::vector<std::unique_ptr<ASTNode>> Statements;

  /// Both sequential or conditional successor. If there's no conditional
  /// successors, second element is empty.
  SuccessorPair Successors;

  /// Arbitrary length set of predecessors.
  std::vector<std::shared_ptr<CFGBlock>> Predecessors;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_BLOCK_HPP