#include "MiddleEnd/CodeGen/CodeEmitter.hpp"
#include "MiddleEnd/IR/Instruction.hpp"
#include "TestHelpers.hpp"

using namespace weak::middleEnd;
using namespace weak::frontEnd;

int main() {
  SECTION(IRBasic) {
    CodeEmitter E;

    Instruction I(/*LabelNo=*/0U, TokenType::STAR, 3, 4);
    E.Emit(TokenType::PLUS, 1, 2);
    E.EmitGotoLabel(1);
    E.Emit(TokenType::MINUS, I, I);
    E.EmitIf(TokenType::EQ, I, 10, /*Label*/1);
    E.Emit(TokenType::STAR, I, 0xFFF);
    E.EmitJump(/*Label*/1);
    E.Dump();
  }
}