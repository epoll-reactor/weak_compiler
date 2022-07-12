#include "FrontEnd/Lex/Lexer.hpp"
#include "FrontEnd/Parse/Parser.hpp"
#include "MiddleEnd/CodeGen/CodeGen.hpp"
#include "MiddleEnd/Symbols/Storage.hpp"
#include "TestHelpers.hpp"

namespace fe = weak::frontEnd;
namespace me = weak::middleEnd;

void CreateCode(std::string_view Program) {
  me::Storage Storage;
  fe::Lexer Lex(&Storage, Program.begin(), Program.end());
  auto Tokens = Lex.Analyze();
  fe::Parser Parser(&*Tokens.begin(), &*Tokens.end());
  auto AST = Parser.Parse();
  me::CodeGen CodeGen(AST.get());
  CodeGen.CreateCode();
}

int main() {
  SECTION(BasicNoReturn) {
    CreateCode("void f(int lhs, float rhs) {\n"
               "  int res = 1 + 2 * lhs * rhs / lhs * rhs;\n"
               "}\n");
  }
  SECTION(BasicReturnInt) {
    CreateCode("int f(float lhs, int rhs) {\n"
               "  return lhs / rhs;\n"
               "}\n");
  }
  SECTION(BasicReturnFloat) {
    CreateCode("float f(float lhs, float rhs) {\n"
               "  return 1 + 2 * lhs * rhs / lhs * rhs;\n"
               "}\n");
  }
  SECTION(Call) {
    CreateCode("int f1(int arg) { return arg + 1 + 2 + 3; }\n"
               "int f2(int arg) { return f1(arg + 2 + arg); }\n");
  }
}