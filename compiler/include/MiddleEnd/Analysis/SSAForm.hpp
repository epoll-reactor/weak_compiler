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

/// \brief SSA implementation.
///
/// Beautifully described here:
/// https://www.cs.utexas.edu/~pingali/CS380C/2010/papers/ssaCytron.pdf
class SSAForm {
public:
  SSAForm(CFG *);

  void Compute(std::string_view Variable);

private:
  void Compute(CFGBlock *Block, std::string_view Variable);

  CFG *CFGraph;
  int SSAIndex;
  std::stack<int> IndicesStack;
  VariableSearchVisitor VariableSearcher;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_ANALYSIS_SSA_FORM_HPP
