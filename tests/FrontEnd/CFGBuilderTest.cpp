#include "FrontEnd/Analysis/CFGBuilder.hpp"
#include "FrontEnd/AST/ASTPrettyPrint.hpp"
#include "FrontEnd/Lex/Lexer.hpp"
#include "FrontEnd/Parse/Parser.hpp"
#include "FrontEnd/Symbols/Storage.hpp"
#include "TestHelpers.hpp"

using namespace weak::frontEnd;

static Lexer CreateLexer(Storage *S, std::string_view Input) {
  Lexer Lex(S, Input.begin(), Input.end());
  return Lex;
}

static void CreateCFG(std::string_view String) {
  Storage Storage;
  auto Tokens = CreateLexer(&Storage, String).Analyze();
  Parser Parse(&*Tokens.begin(), &*Tokens.end());
  auto AST = Parse.Parse();
  CFGBuilder Builder(std::move(AST));
  auto CFG = Builder.BuildCFG();
}

int main() {
  SECTION(CFGCompoundLoops) {
    CreateCFG("void f() {"
              "  while (1) {"
              "    if (2) {"
              "      do_if(3);"
              "    } else {"
              "      do_else(4);"
              "    }"
              "  }"
              "  do {"
              "    if (5) {"
              "      do_if(6);"
              "    } else {"
              "      do_else(7);"
              "    }"
              "  } while(8);"
              "  do_after(9);"
              "}");
  }

  SECTION(CFGForLoop) {
    CreateCFG("void f() {"
              "  for (init; cond; inc) {"
              "    do_for_body(0);"
              "  }"
              "  do_after_for(0);"
              "}");
  }

  SECTION(CFGBasic) {
    CreateCFG("void f() {"
              "  if (1) {"
              "    call(2);"
              "  } else {"
              "    call(3);"
              "  }"
              "  call(4);"
              "  call(5);"
              "}");
  }
  SECTION(CFGNestedIf) {
    CreateCFG("void f() {"
              "  if (1) {"
              "    a = 2;"
              "    a = 22;"
              "    a = 222;"
              "    if (3) {"
              "      b = 4;"
              "      if (5) {"
              "        c = 6;"
              "      }"
              "    }"
              "  }"
              "  d = 7;"
              "}");
  }
  SECTION(CFGDeepNestedIfElse) {
    CreateCFG("void f() {"
              "  for (a; b; c) {"
              "    if (1) {"
              "      call(2);"
              "      call(3);"
              "      call(4);"
              "      if (5) {"
              "         call(6);"
              "       if (7) {"
              "         call(8);"
              "       } else {"
              "         call(9);"
              "       }"
              "      }"
              "    } else {"
              "      call(10);"
              "    }"
              "    call(11);"
              "  }"
              "}");
  }
  SECTION(CFGForLoop) {
    CreateCFG("void f() {"
              "  f(0);"
              "  for (a; b; c) {"
              "    for (d; e; f) {"
              "      f(a);"
              "    }"
              "    f(b);"
              "  }"
              "  f(c);"
              "}");
  }
  SECTION(CFGWhileLoop) {
    CreateCFG("void f() {"
              "  while (var) {"
              "    f(2);"
              "  }"
              "  f(3);"
              "}");
  }
  SECTION(CFGDoWhileLoop) {
    CreateCFG("void f() {"
              "  do {"
              "    f(1); f(2);"
              "  } while (var);"
              "  f(4);"
              "}");
  }

  SECTION(CFGCompoundLoops) {
    CreateCFG("void f() {"
              "  stmt++;"
              "  for (a; b; c) {"
              "    if (x) { y++; } else { z++; }"
              "    for (d; e; f) {"
              "      var2++;"
              "      for (x; y; z) {"
              "        do_for(5);"
              "      }"
              "      stmt3++;"
              "    }"
              "    stmt4++;"
              "  }"
              "  do_after(6);"
              "}");
  }
}