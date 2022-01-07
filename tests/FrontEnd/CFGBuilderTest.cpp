#include "FrontEnd/Analysis/CFGBuilder.hpp"
#include "FrontEnd/Lex/Lexer.hpp"
#include "FrontEnd/Parse/Parser.hpp"
#include "../TestHelpers.hpp"

using namespace weak::frontEnd;

static Lexer CreateLexer(std::string_view Input) {
  Lexer Lex(Input.begin(), Input.end());
  return Lex;
}

static void CreateCFG(std::string_view String) {
  auto Tokens = CreateLexer(String).Analyze();
  Parser Parse(&*Tokens.begin(), &*Tokens.end());
  auto AST = Parse.Parse();
  CFGBuilder CFG(std::move(AST));
  CFG.BuildCFG();
}

int main() {
  SECTION(CFGBasic) {
    CreateCFG("void f() {"
              "  a = 2;"
              "  if (1) { b = 3; } else { d = 4; }"
              "}");
  }
}