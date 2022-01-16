#include "FrontEnd/Lex/Lexer.hpp"
#include "FrontEnd/Parse/Parser.hpp"
#include "FrontEnd/Symbols/Storage.hpp"
#include "MiddleEnd/CodeGen/CodeGen.hpp"
#include "TestHelpers.hpp"

namespace fe = weak::frontEnd;
namespace me = weak::middleEnd;

void RunCodeGenTest(std::string_view Program) {
  fe::Storage Storage;
  auto Tokens = fe::Lexer(&Storage, &*Program.begin(), &*Program.end()).Analyze();
  auto AST = fe::Parser(Tokens.begin().base(), Tokens.end().base()).Parse();
  me::CodeGen(AST.get()).CreateCode();
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
                   "  if (0) {"
                   "    if (1) {"
                   "      if (2) {"
                   "        if (3) {"
                   "          if (4) {"
                   "            int a = 10 % 1;"
                   "          }"
                   "          int b = 9 % 1;"
                   "        }"
                   "        int c = 8 % 1;"
                   "      } else { int else_body = 1 + 2; }"
                   "      int d = 7 % 1;"
                   "    }"
                   "    int e = 6 % 1;"
                   "  }"
                   "  int f = 5 % 1;"
                   "}");
  }
}