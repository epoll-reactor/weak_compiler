#ifndef WEAK_COMPILER_FRONTEND_AST_AST_NODE_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_NODE_HPP

namespace weak {
namespace frontEnd {

class ASTNode {
public:
  virtual ~ASTNode() noexcept = default;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_NODE_HPP
