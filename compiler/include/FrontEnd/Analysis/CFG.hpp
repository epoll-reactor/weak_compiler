#ifndef WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_HPP
#define WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_HPP

#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/Analysis/CFGBlock.hpp"
#include "Utility/Uncopyable.hpp"
#include "Utility/Unmovable.hpp"
#include <vector>

namespace weak {
namespace frontEnd {
/*! This is an implementation of Control Flow Graph, which forms from AST. */
class CFG : public Uncopyable, public Unmovable {
public:
  CFG(CFGBlock *TheRootBlock);

private:
  CFGBlock *RootBlock;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_ANALYSIS_CFG_HPP