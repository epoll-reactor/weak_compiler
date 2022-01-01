#include "FrontEnd/Lex/Lexer.hpp"
#include "FrontEnd/AST/ASTPrettyPrint.hpp"
#include "FrontEnd/Parse/Parser.hpp"

using namespace weak::frontEnd;

static Lexer CreateLexer(std::string_view Input) {
  Lexer Lex(Input.begin(), Input.end());
  return Lex;
}

static void PrintAST(std::string_view String) {
  auto Tokens = CreateLexer(String).Analyze();
  Parser Parse(&*Tokens.begin(), &*Tokens.end());
  ASTPrettyPrint(Parse.Parse());
}

int main() {
  PrintAST("void main(string symbol, int integer) {"
           "  for (a;a;a) { a; a; a; }"
           "  while (a) { a; a; a; }"
           "  do { a; b; c; } while (a)"
           "}");
}