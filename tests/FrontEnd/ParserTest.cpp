#include "FrontEnd/Lex/Lexer.hpp"
#include "FrontEnd/Parse/Parser.hpp"

using namespace weak::frontEnd;

static Lexer CreateLexer(std::string_view Input) {
  Lexer Lex(Input.begin(), Input.end());
  return Lex;
}

int main() {
  auto Tokens = CreateLexer("++ ++ ++").Analyze();
  Parser Parse(&*Tokens.begin(), &*Tokens.end());
  Parse.Parse();
}