#include "FrontEnd/AST/ASTPrettyPrint.hpp"
#include "ASTMakeFunctions.hpp"

using weak::frontEnd::TokenType;

int main() {
  ASTPrettyPrint(MakeBinary(
      TokenType::PLUS, MakeInteger(1),
      MakeBinary(
          TokenType::PLUS, MakeInteger(2),
          MakeBinary(TokenType::PLUS, MakeInteger(3),
                     MakeBinary(TokenType::PLUS,
                                MakeUnary(ASTUnaryOperator::UnaryType::POSTFIX,
                                          TokenType::INC, MakeFloat(123.123)),
                                MakeString("123"))))));

  ASTPrettyPrint(MakeUnary(ASTUnaryOperator::UnaryType::POSTFIX, TokenType::INC,
                           MakeString("Some text")));
}
