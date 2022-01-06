#include "ASTMakeFunctions.hpp"
#include "FrontEnd/Analysis/CFGBlock.hpp"
#include "../TestHelpers.hpp"
#include <cassert>

using namespace weak::frontEnd;

using namespace weak::frontEnd;

static_assert(std::is_move_constructible_v<CFGBlock>);
static_assert(std::is_move_assignable_v<CFGBlock>);
static_assert(!std::is_copy_constructible_v<CFGBlock>);
static_assert(!std::is_copy_assignable_v<CFGBlock>);

auto CreateCFGBlock() {
  auto Block = std::make_shared<CFGBlock>();
  Block->AddStatement(MakeInteger(1));
  Block->AddStatement(MakeInteger(2));
  return Block;
}

int main() {
  SECTION(CFGBasicLinks) {
    std::shared_ptr<CFGBlock> Block1 = CreateCFGBlock();
    std::shared_ptr<CFGBlock> Block2 = CreateCFGBlock();

    assert(!Block1->LinkedWith(Block2));
    assert(!Block2->LinkedWith(Block1));

    assert(Block1->GetSuccessors().size() == 0);
    assert(Block1->GetPredecessors().size() == 0);

    Block1->Link(Block2);
    assert(Block1->GetSuccessors().size() == 1);
    assert(Block2->GetPredecessors().size() == 1);

    assert(Block1->LinkedWith(Block2));

    Block1->Unlink(Block2);
    assert(Block1->GetSuccessors().size() == 0);
    assert(Block2->GetPredecessors().size() == 0);

    assert(!Block1->LinkedWith(Block2));
  }
  SECTION(CFGEnsureUniqueLinks) {
    std::shared_ptr<CFGBlock> Block1 = CreateCFGBlock();
    std::shared_ptr<CFGBlock> Block2 = CreateCFGBlock();
    std::shared_ptr<CFGBlock> Block3 = CreateCFGBlock();

    Block1->Link(Block2);
    assert(Block1->GetSuccessors().size() == 1);
    assert(Block2->GetPredecessors().size() == 1);
    assert(Block1->GetPredecessors().size() == 0);
    assert(Block2->GetSuccessors().size() == 0);

    Block2->Link(Block3);
  }
  SECTION(CFGCyclicLink) {
    std::shared_ptr<CFGBlock> Block1 = CreateCFGBlock();
    std::shared_ptr<CFGBlock> Block2 = CreateCFGBlock();

    Block1->Link(Block2);
    Block2->Link(Block1);
    assert(Block1->GetPredecessors().size() == 1);
    assert(Block1->GetSuccessors().size() == 1);
    assert(Block2->GetPredecessors().size() == 1);
    assert(Block2->GetSuccessors().size() == 1);
  }
}