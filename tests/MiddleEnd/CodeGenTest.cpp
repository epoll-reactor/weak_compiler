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
  SECTION(BasicCodeGen) {
    RunCodeGenTest("void f() {"
                   "  int a = 1 * 2 + 3 * 4 * 5 + 6 ^ 7 & 8 & 9;"
                   "}");
  }
}