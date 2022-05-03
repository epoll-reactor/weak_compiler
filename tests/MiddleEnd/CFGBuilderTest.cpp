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

  CFGBuilder Builder(AST->GetStmts());
  Builder.Build();

  std::ofstream("CFG.gv") << CFGToDot(&Builder.GetCFG());
  system("dot -Tjpg CFG.gv -o CFG.jpg && sxiv CFG.jpg");
  std::remove("CFG.gv");
}

int main() {
//  CreateCFG("void f() {"
//            "  int a = 1;"
//            "  int b = 2;"
//            "  int const = 777;"
//            "  if (1 + 2) {"
//            "    int c = a + b + 2;"
//            "    a = c;"
//            "    b = c;"
//            "    if (3 + 4) {"
//            "      c = a;"
//            "      b = const;"
//            "    } else {"
//            "      b = 888;"
//            "    }"
//            "    int result = const;"
//            "  }"
//            "  a = 7;"
//            "  b = 8;"
//            "}");

//  CreateCFG("void f() {"
//            "  int a = 1;"
//            "  int b = 2;"
//            "  while (a < b) {"
//            "    a = 3;"
//            "    b = 4;"
//            "    while (5 + 6) {"
//            "      a = 7;"
//            "      b = 8;"
//            "      while (9 + 10) {"
//            "        a = 11;"
//            "        b = 12;"
//            "      }"
//            "    }"
//            "    if (13 + 14) {"
//            "      a = b;"
//            "      b = a;"
//            "    }"
//            "  }"
//            "  a = 15;"
//            "  b = 16;"
//            "}");

//  CreateCFG("void f() {"
//            "  int a = 1;"
//            "  int b = 2;"
//            "  do {"
//            "    a = 3;"
//            "    b = 4;"
//            "    do {"
//            "      a = 7;"
//            "      b = 8;"
//            "      while (9 + 10) {"
//            "        a = 11;"
//            "        b = 12;"
//            "      }"
//            "    } while (5 + 6);"
//            "    if (13 + 14) {"
//            "      a = b;"
//            "      b = a;"
//            "    } else {"
//            "      b = a;"
//            "      a = b;"
//            "    }"
//            "  } while (a < b);"
//            "  a = 15;"
//            "  b = 16;"
//            "}");

  CreateCFG("void f() {"
            "  int a = 1;"
            "  int b = 2;"
            "  for (int i = 0; i < 10; i = i + 1) {"
            "    do {"
            "      a = 7;"
            "      b = 8;"
            "      while (9 + 10) {"
            "        a = 11;"
            "        b = 12;"
            "      }"
            "    } while (5 + 6);"
            "    a = a + i;"
            "    b = b - i;"
            "  }"
            "  a = b;"
            "  b = a;"
            "}");
}