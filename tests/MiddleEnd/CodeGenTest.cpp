#include "FrontEnd/Lex/Lexer.hpp"
#include "FrontEnd/Parse/Parser.hpp"
#include "MiddleEnd/Symbols/Storage.hpp"
#include "MiddleEnd/CodeGen/CodeGen.hpp"
#include "TestHelpers.hpp"

namespace fe = weak::frontEnd;
namespace me = weak::middleEnd;

void RunCodeGenTest(std::string_view Program) {
  me::Storage Storage;
  auto Tokens = fe::Lexer(&Storage, &*Program.begin(), &*Program.end()).Analyze();
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
  SECTION(VariableNotFound) {
//    RunCodeGenTest("void f() {"
//                   "  int c = b + a;"
//                   "}");
  }
}