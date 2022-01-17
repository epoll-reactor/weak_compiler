#include "MiddleEnd/CodeGen/CodeGen.hpp"
#include "FrontEnd/Lex/Lexer.hpp"
#include "FrontEnd/Parse/Parser.hpp"
#include "MiddleEnd/Symbols/Storage.hpp"
#include "TestHelpers.hpp"

namespace fe = weak::frontEnd;
namespace me = weak::middleEnd;

void RunCodeGenTest(std::string_view Program) {
  me::Storage Storage;
  auto Tokens =
      fe::Lexer(&Storage, &*Program.begin(), &*Program.end()).Analyze();
  auto AST = fe::Parser(Tokens.begin().base(), Tokens.end().base()).Parse();
  me::CodeGen(&Storage, AST.get()).CreateCode();
}

int main() {
  SECTION(Binary) {
    RunCodeGenTest("void f() {"
                   "  int a = 1 * 2 + 3 * 4 * 5 + 6 ^ 7 & 8 & 9;"
                   "}");
  }
  SECTION(Unary) {
    RunCodeGenTest("void f() {"
                   "  int a =  1++ + ++1;"
                   "}");
  }
  SECTION(If) {
    RunCodeGenTest("void f() {"
                   "  if (1) {"
                   "    int a = 2 + 3;"
                   "  }"
                   "  int b = 4 + 5;"
                   "}");
  }
  SECTION(IfElse) {
    RunCodeGenTest("void f() {"
                   "  if (2) {"
                   "    int a = 3 + 4;"
                   "  } else {"
                   "    int b = 5 + 6;"
                   "  }"
                   "  int c = 7 + 8;"
                   "}");
  }
  SECTION(DeepNestedIf) {
    RunCodeGenTest("void f() {"
                   "  if (true) {"
                   "    if (false) {"
                   "      if (true) {"
                   "        if (false) {"
                   "          if (true) {"
                   "            int a = 10.11 - 0.99;"
                   "          }"
                   "          int b = 9.99 - 0.99;"
                   "        }"
                   "        int c = 8.88 - 0.99;"
                   "      } else { int else_body = 1 + 2; }"
                   "      int d = 7.77 - 0.99;"
                   "    }"
                   "    int e = 6.66 - 0.99;"
                   "  }"
                   "  int f = 5.55 - 0.99;"
                   "}");
  }
  SECTION(While) {
    /*!
     *  L0:  if 1 != 0 goto L13
     *  L1:  if 2 != 0 goto L11
     *  L2:  if 3 != 0 goto L9
     *  L3:  if 4 != 0 goto L7
     *  L4:  t1 = 4 + 5
     *  L5:  t2 = 6 + 7
     *  L6:  goto L3
     *  L7:  t3 = 8 + 9
     *  L8:  goto L2
     *  L9:  t4 = 10 + 11
     *  L10: goto L1
     *  L11: t5 = 12 + 13
     *  L12: goto L0
     *  L13: t6 = 14 + 15
     */
    RunCodeGenTest("void f() {"
                   "  while (1) {"
                   "    while (2) {"
                   "      while (3) {"
                   "        while (4) {"
                   "          int var = 4 + 5;"
                   "          int var = 6 + 7;"
                   "        }"
                   "        int var = 8 + 9;"
                   "      }"
                   "      int var = 10 + 11;"
                   "    }"
                   "    int var = 12 + 13;"
                   "  }"
                   "  int var = 14 + 15;"
                   "}");
  }
    SECTION(Variables) {
      RunCodeGenTest("void f() {"
                     "  int a = 2++ + ++2;"
                     "  int b = 3 + 4;"
                     "  int c = b + a;"
                     "  int d = c + a;"
                     "  int e = d + a;"
                     "  int f = e + a;"
                     "}");
    }
  SECTION(ComplexTest) {
    RunCodeGenTest("void f() {"
                   "  int factorial = 1;"
                   "  int i = 1;"
                   "  while (i < 5) {"
                   "    factorial = factorial * i;"
                   "  }"
                   "}");
  }
}