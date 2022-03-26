#include "FrontEnd/Parse/Parser.hpp"
#include "FrontEnd/AST/ASTPrettyPrint.hpp"
#include "FrontEnd/Lex/Lexer.hpp"
#include "MiddleEnd/Symbols/Storage.hpp"
#include "MiddleEnd/Analysis/CFGBuilder.hpp"
#include <fstream>
#include <iostream>

using namespace weak::frontEnd;
using namespace weak::middleEnd;

static Lexer CreateLexer(Storage *S, std::string_view Input) {
  Lexer Lex(S, Input.begin(), Input.end());
  return Lex;
}

static void CreateCFG(std::string_view String) {
  Storage Storage;
  auto Tokens = CreateLexer(&Storage, String).Analyze();
  Parser Parse(&*Tokens.begin(), &*Tokens.end());
  auto AST = Parse.Parse();

  for (const auto &Stmt : AST->GetStmts())
    ASTPrettyPrint(Stmt);

  CFGBuilder Builder;
  for (const auto &Expr : AST->GetStmts())
    Builder.Traverse(Expr.get());

  Builder.CommitSSAFormBuilding();

  std::ofstream("CFG.gv") << CFGToDot(Builder.GetCFG());
  system("dot -Tjpg CFG.gv -o CFG.jpg && sxiv CFG.jpg");
  std::remove("CFG.gv");
}

int main() {
  CreateCFG("void f() {"
            "  int a = 1;"
            "  int b = 2;"
            "  int const = 777;"
            "  if (1 + 2) {"
            "    int c = a + b + 2;"
            "    a = c;"
            "    b = c;"
            "    if (3 + 4) {"
            "      c = a;"
            "      b = const;"
            "    } else {"
            "      b = 888;"
            "    }"
            "    int result = const;"
            "  }"
            "  a = 7;"
            "  b = 8;"
            "}");

//  CreateCFG("void f() {"
//            "  int a = 1;"
//            "  while (a < 10) {"
//            "    int b = a;"
//            "    a = b;"
//            "  }"
//            "  a = 123 + a;"
//            "}");

//  CreateCFG("void f() {"
//            "  int a = 1;"
//            "  int b = 2;"
//            "  int c = 3;"
//            "  int d = 4;"
//            "  while (a + b < c + d) {"
//            "    int result = a + b + c + d;"
//            "    a = result + 1;"
//            "    b = result + 2;"
//            "    c = result + 3;"
//            "    d = result + 4;"
//            "  }"
//            "  d = c;"
//            "  c = b;"
//            "  b = a;"
//            "  a = a + b;"
//            "}");
}