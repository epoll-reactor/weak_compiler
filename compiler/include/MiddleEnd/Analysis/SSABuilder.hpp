#ifndef WEAK_COMPILER_MIDDLE_END_ANALYSIS_SSA_HPP
#define WEAK_COMPILER_MIDDLE_END_ANALYSIS_SSA_HPP

#include "MiddleEnd/Analysis/CFG.hpp"
#include "MiddleEnd/IR/VariableSearchVisitor.hpp"
#include <stack>

namespace weak {
namespace middleEnd {

class SSABuilder {
public:
  SSABuilder(CFG *);

  void VariableToSSA(std::string_view VarName);

private:
  int Counter;
  std::stack<int> Stack;
  VariableSearchVisitor VarSearcher;

  void TraverseWithRespectToVariable(CFGBlock *Block, std::string_view VarName);

  CFG *CFGraph;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_ANALYSIS_SSA_HPP
