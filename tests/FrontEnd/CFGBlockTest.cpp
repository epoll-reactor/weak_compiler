#include "ASTMakeFunctions.hpp"
#include "FrontEnd/Analysis/CFGBlock.hpp"
#include "TestHelpers.hpp"
#include <cassert>

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

    assert(!Block1->IsSuccessorOf(Block2));
    assert(!Block2->IsPredecessorOf(Block1));

    assert(!Block1->GetSequentialSuccessor());
    assert(!Block1->GetConditionalSuccessor());
    assert(Block1->GetPredecessors().size() == 0);

    Block1->AddSequentialSuccessor(Block2);
    assert(Block2->GetPredecessors().size() == 1);
    assert(Block1->IsPredecessorOf(Block2));
    assert(Block2->IsSuccessorOf(Block1));
    assert(!Block1->IsSuccessorOf(Block1));
    assert(!Block1->IsSuccessorOf(Block2));

    assert(Block1->GetSequentialSuccessor());
  }
  SECTION(CFGConditionSuccessor) {
    std::shared_ptr<CFGBlock> MainBlock = CreateCFGBlock();
    std::shared_ptr<CFGBlock> YesBlock = CreateCFGBlock();
    std::shared_ptr<CFGBlock> NoBlock = CreateCFGBlock();

    assert(!MainBlock->GetSequentialSuccessor());
    assert(!MainBlock->GetConditionalSuccessor());

    assert(!MainBlock->IsPredecessorOf(YesBlock));
    assert(!MainBlock->IsPredecessorOf(NoBlock));
    assert(!YesBlock->IsSuccessorOf(MainBlock));
    assert(!NoBlock->IsSuccessorOf(MainBlock));

    MainBlock->AddSequentialSuccessor(YesBlock);
    MainBlock->AddConditionSuccessor(NoBlock);

    assert(MainBlock->IsPredecessorOf(YesBlock));
    assert(MainBlock->IsPredecessorOf(NoBlock));
    assert(YesBlock->IsSuccessorOf(MainBlock));
    assert(NoBlock->IsSuccessorOf(MainBlock));

    assert(MainBlock->GetSequentialSuccessor());
    assert(MainBlock->GetConditionalSuccessor());
    assert(YesBlock->GetPredecessors().size() == 1);
    assert(NoBlock->GetPredecessors().size() == 1);
  }
}
