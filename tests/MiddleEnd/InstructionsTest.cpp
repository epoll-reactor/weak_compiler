#include "MiddleEnd/CodeGen/CodeEmitter.hpp"
#include "MiddleEnd/IR/Instruction.hpp"
#include "TestHelpers.hpp"

using namespace weak::middleEnd;
using namespace weak::frontEnd;

int main() {
  SECTION(IRBasic) {
    CodeEmitter E;

    E.BeginFunction("F1", {TokenType::INT});
    Instruction I(/*LabelNo=*/0U, TokenType::STAR, 3, 4);
    E.Emit(TokenType::PLUS, 1, 2);
    E.Emit(TokenType::PLUS, 1.23, 4.56);
    E.EmitGotoLabel(1);
    E.Emit(TokenType::MINUS, I, I);
    E.EmitIf(TokenType::EQ, I, 10, /*Label*/ 1);
    E.Emit(TokenType::STAR, I, 0xFFF);
    E.Emit(1);
    E.EmitJump(/*Label*/ 1);
    E.EmitCall("F1", {});
    E.EmitCall("F2", {
      Reference(Instruction(0, TokenType::PLUS, 1, 1)),
      Reference(Instruction(1, TokenType::PLUS, 1, 1)),
      Reference(Instruction(2, TokenType::PLUS, 1, 1)),
      Reference(Instruction(3, TokenType::PLUS, 1, 1))
    });
    E.EndFunction();
    E.Dump();
  }
}