#include "ASTMakeFunctions.hpp"
#include "FrontEnd/Analysis/CFGBlock.hpp"
#include <cassert>

using namespace weak::frontEnd;

CFGBlock CreateCFGBlock() {
  using ASTVector = std::vector<std::unique_ptr<ASTNode>>;

  ASTVector Block1Stmts;
  Block1Stmts.push_back(MakeInteger(1));
  Block1Stmts.push_back(MakeInteger(2));

  return CFGBlock{std::move(Block1Stmts)};
}

int main() {
  CFGBlock Block1 = CreateCFGBlock();
  CFGBlock Block2 = CreateCFGBlock();

  assert(!Block1.LinkedWith(Block2));
  assert(!Block2.LinkedWith(Block1));

  assert(Block1.GetSuccessors().size() == 0);
  assert(Block1.GetPredecessors().size() == 0);

  Block1.Link(Block2);
  assert(Block1.GetSuccessors().size() == 1);
  assert(Block2.GetPredecessors().size() == 1);

  assert(Block1.LinkedWith(Block2));

  Block1.Unlink(Block2);
  assert(Block1.GetSuccessors().size() == 0);
  assert(Block2.GetPredecessors().size() == 0);

  assert(!Block1.LinkedWith(Block2));
}