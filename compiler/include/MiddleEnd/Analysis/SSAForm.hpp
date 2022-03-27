/* SSABuilder.hpp - Constructor of SSA form from CFG.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_ANALYSIS_SSA_FORM_HPP
#define WEAK_COMPILER_MIDDLE_END_ANALYSIS_SSA_FORM_HPP

#include "MiddleEnd/Analysis/CFG.hpp"
#include "MiddleEnd/IR/VariableSearchVisitor.hpp"
#include <stack>

namespace weak {
namespace middleEnd {

class SSAForm {
public:
  SSAForm(CFG *);

  void VariableToSSA(std::string_view Variable);

private:
  int CurrentSSAIndex;
  std::stack<int> IndicesStack;
  VariableSearchVisitor VariableSearcher;

  void TraverseWithRespectToVariable(CFGBlock *Block,
                                     std::string_view Variable);

  CFG *CFGraph;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_ANALYSIS_SSA_FORM_HPP
