#include "MiddleEnd/CodeGen/CodeGen.hpp"
#include "FrontEnd/Lex/Lexer.hpp"
#include "FrontEnd/Parse/Parser.hpp"
#include "MiddleEnd/Symbols/Storage.hpp"
#include "Utility/Diagnostic.hpp"
#include "TestHelpers.hpp"
#include <cassert>

namespace fe = weak::frontEnd;
namespace me = weak::middleEnd;

void RunCodeGenTest(std::string_view Program, std::list<weak::middleEnd::AnyInstruction> &&Assert) {
  me::Storage Storage;
  auto Tokens =
      fe::Lexer(&Storage, &*Program.begin(), &*Program.end()).Analyze();
  auto AST = fe::Parser(Tokens.begin().base(), Tokens.end().base()).Parse();
  auto Code = me::CodeGen(&Storage, AST.get()).CreateCode();

  assert(!Code.empty());
  assert(!Assert.empty());
  assert(Code.size() == Assert.size());

  auto CodeIt = Code.begin();
  auto AssertIt = Assert.begin();

  while (CodeIt != Code.end()) {
    if (*CodeIt != *AssertIt) {
      weak::DiagnosticError() << "Assertion failed:\n" << *CodeIt
                              << " got,\n" << *AssertIt << " expected, "
                              << "showing code...\n";
      me::CodeEmitter::Dump(Code);
      weak::UnreachablePoint();
    }

    ++CodeIt;
    ++AssertIt;
  }

//  assert(Code == Assert);
}

int main() {
  using namespace weak::middleEnd;
  using namespace weak::frontEnd;
  using Unary = UnaryInstruction;
  using Instr = Instruction;
  using If = IfInstruction;
  using Ref = InstructionReference;
  using Label = GotoLabel;
  using Jump = Jump;
  SECTION(Binary) {
    Instr T0(0, TokenType::STAR, 1, 2);
    Instr T1(1, TokenType::STAR, 4, 5);
    Instr T2(2, TokenType::STAR, 3, Ref(T1));
    Instr T3(3, TokenType::PLUS, Ref(T2), 6);
    Instr T4(4, TokenType::PLUS, Ref(T0), Ref(T3));
    Instr T5(5, TokenType::BIT_AND, 8, 9);
    Instr T6(6, TokenType::BIT_AND, 7, Ref(T5));
    Instr T7(7, TokenType::XOR, Ref(T4), Ref(T6));
    RunCodeGenTest("void f() {"
                   "  int a = 1 * 2 + 3 * 4 * 5 + 6 ^ 7 & 8 & 9;"
                   "}", {
      T0, T1, T2, T3, T4, T5, T6, T7
    });
  }
  SECTION(Unary) {
    Instr T0(0, TokenType::PLUS, 1, 1);
    Instr T1(1, TokenType::PLUS, 1, 1);
    Instr T2(2, TokenType::PLUS, T0, T1);
    RunCodeGenTest("void f() {"
                   "  int a = 1++ + ++1;"
                   "}", {
      T0, T1, T2
    });
  }
//  SECTION(OperandSize) {
//    \todo Fix boolean assignment.
//    RunCodeGenTest("void f() {"
//                   "  int a = 3;"
//                   "  bool b = true;"
//                   "  int c = a + b;"
//                   "}", {});
//  }
  SECTION(If) {
    If If1(TokenType::NEQ, 1, 0, 0);
    Jump J1(1);
    Label L0(0);
    Instr T0(0, TokenType::PLUS, 2, 3);
    Label L1(1);
    Instr T1(1, TokenType::PLUS, 4, 5);
    RunCodeGenTest("void f() {"
                   "  if (1) {"
                   "    int a = 2 + 3;"
                   "  }"
                   "  int b = 4 + 5;"
                   "}", {
      If1, J1, L0, T0, L1, T1
    });
  }
  SECTION(IfElse) {
    If If1(TokenType::LT, 1, 1, 0);
    Jump J3(3);
    Label L0(0);
    If If2(TokenType::LT, 2, 2, 1);
    Jump J2(2);
    Label L1(1);
    Instr T0(0, TokenType::PLUS, 3, 3);
    Label L2(2);
    Instr T1(1, TokenType::PLUS, 4, 4);
    Label L3(3);
    Instr T2(2, TokenType::PLUS, 5, 5);
    Instr T3(3, TokenType::PLUS, 6, 6);
    RunCodeGenTest("void f() {"
                   "  if (1 < 1) {"
                   "    if (2 < 2) {"
                   "      int a = 3 + 3;"
                   "    }"
                   "    int b = 4 + 4;"
                   "  } else {"
                   "    int c = 5 + 5;"
                   "  }"
                   "  int d = 6 + 6;"
                   "}",{
      If1, J3, L0, If2, J2, L1, T0, L2, T1, L3, T2, T3
    });
  }
  SECTION(DeepNestedIf) {
    If If0(TokenType::NEQ, true, 0, 0);
    Jump J9(9);
    Label L0(0);
    If If1(TokenType::NEQ, false, 0, 1);
    Jump J8(8);
    Label L1(1);
    If If2(TokenType::NEQ, true, 0, 2);
    Jump J7(7);
    Label L2(2);
    If If3(TokenType::NEQ, false, 0, 3);
    Jump J6(6);
    Label L3(3);
    If If4(TokenType::NEQ, true, 0, 4);
    Jump J5(5);
    Label L4(4);
    Instr T0(0, TokenType::MINUS, 10.11, 0.99);
    Label L5(5);
    Instr T1(1, TokenType::MINUS, 9.99, 0.99);
    Label L6(6);
    Instr T2(2, TokenType::MINUS, 8.88, 0.99);
    Label L7(7);
    Instr T3(3, TokenType::PLUS, 1, 2);
    Instr T4(4, TokenType::MINUS, 7.77, 0.99);
    Label L8(8);
    Instr T5(5, TokenType::MINUS, 6.66, 0.99);
    Label L9(9);
    Instr T6(6, TokenType::MINUS, 5.55, 0.99);
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
                   "}", {
      If0, J9, L0, If1, J8, L1, If2, J7, L2, If3, J6, L3, If4, J5, L4, T0, L5, T1,
      L6, T2, L7, T3, T4, L8, T5, L9, T6
    });
  }
  SECTION(While) {
    Label L0(0);
    If If1(TokenType::LT, 1, 1, 1);
    Jump J4(4);
    Label L1(1);
    If If2(TokenType::LT, 2, 2, 2);
    Jump J3(3);
    Label L2(2);
    Instr T0(0, TokenType::PLUS, 4, 5);
    Instr T1(1, TokenType::PLUS, 6, 7);
    Jump J1(1);
    Label L3(3);
    Instr T2(2, TokenType::PLUS, 12, 13);
    Jump J0(0);
    Label L4(4);
    Instr T3(3, TokenType::PLUS, 14, 15);
    RunCodeGenTest("void f() {"
                   "  while (1 < 1) {"
                   "    while (2 < 2) {"
                   "      int var = 4 + 5;"
                   "      int var = 6 + 7;"
                   "    }"
                   "    int var = 12 + 13;"
                   "  }"
                   "  int var = 14 + 15;"
                   "}", {
      L0, If1, J4, L1, If2, J3, L2, T0, T1, J1, L3, T2, J0, L4, T3
    });
  }
  SECTION(DeepNestedWhile) {
    Label L0(0);
    If If1(TokenType::LT, 1, 1, 1);
    Jump J12(12);
    Label L1(1);
    If If2(TokenType::LT, 2, 2, 2);
    Jump J11(11);
    Label L2(2);
    If If3(TokenType::LT, 3, 3, 3);
    Jump J10(10);
    Label L3(3);
    If If4(TokenType::LT, 4, 4, 4);
    Jump J9(9);
    Label L4(4);
    If If5(TokenType::LT, 5, 5, 5);
    Jump J8(8);
    Label L5(5);
    If If6(TokenType::NEQ, 111, 222, 6);
    Jump J7(7);
    Label L6(6);
    Instr T0(0, TokenType::PLUS, 6, 6);
    Label L7(7);
    Instr T1(1, TokenType::PLUS, 999, 999);
    Jump J4(4);
    Label L8(8);
    Instr T2(2, TokenType::PLUS, 7, 7);
    Jump J3(3);
    Label L9(9);
    Instr T3(3, TokenType::PLUS, 8, 8);
    Jump J2(2);
    Label L10(10);
    Instr T4(4, TokenType::PLUS, 9, 9);
    Jump J1(1);
    Label L11(11);
    Instr T5(5, TokenType::PLUS, 10, 10);
    Jump J0(0);
    Label L12(12);
    Instr T6(6, TokenType::PLUS, 11, 11);
    RunCodeGenTest("void f() {"
                   "  while (1 < 1) {"
                   "    while (2 < 2) {"
                   "      while (3 < 3) {"
                   "        while (4 < 4) {"
                   "          while (5 < 5) {"
                   "            if (111 != 222) {"
                   "              int a1 = 6 + 6;"
                   "            }"
                   "            int av = 999 + 999;"
                   "          }"
                   "          int a2 = 7 + 7;"
                   "        }"
                   "        int a3 = 8 + 8;"
                   "      }"
                   "      int a4 = 9 + 9;"
                   "    }"
                   "    int a5 = 10 + 10;"
                   "  }"
                   "  int a6 = 11 + 11;"
                   "}", {
      L0, If1, J12, L1, If2, J11, L2, If3, J10, L3, If4, J9, L4, If5, J8, L5,
      If6, J7, L6, T0, L7, T1, J4, L8, T2, J3, L9, T3, J2, L10, T4, J1, L11, T5,
      J0, L12, T6
    });
  }
  SECTION(DoWhile) {
    Label L0(0);
    Label L1(1);
    Label L2(2);
    Label L3(3);
    Instr T0(0, TokenType::PLUS, 1, 2);
    Instr T1(1, TokenType::PLUS, 3, 4);
    If If4(TokenType::NEQ, 5, 0, 4);
    Jump J3(3);
    Label L4(4);
    Instr T2(2, TokenType::PLUS, 6, 7);
    If If5(TokenType::NEQ, 8, 0, 5);
    Jump J2(2);
    Label L5(5);
    Instr T3(3, TokenType::PLUS, 9, 10);
    If If6(TokenType::NEQ, 11, 0, 6);
    Jump J1(1);
    Label L6(6);
    Instr T4(4, TokenType::PLUS, 12, 13);
    If If7(TokenType::NEQ, 14, 0, 7);
    Jump J0(0);
    Label L7(7);
    Instr T5(5, TokenType::PLUS, 15, 16);
    RunCodeGenTest("void f() {"
                   "  do {"
                   "    do {"
                   "      do {"
                   "        do {"
                   "          int var = 1 + 2;"
                   "          int var = 3 + 4;"
                   "        } while (5);"
                   "        int var = 6 + 7;"
                   "      } while (8);"
                   "      int var = 9 + 10;"
                   "    } while (11);"
                   "    int var = 12 + 13;"
                   "  } while (14);"
                   "  int var = 15 + 16;"
                   "}", {
      L0, L1, L2, L3, T0, T1, If4, J3, L4, T2, If5, J2, L5, T3, If6,
      J1, L6, T4, If7, J0, L7, T5
    });
  }
  SECTION(For) {
    Unary T0(0, 10);
    Unary T1(1, 0);
    Label L0(0);
    If If1(TokenType::LT, T1, 20, 1);
    Jump J2(2);
    Label L1(1);
    Instr T0Change(0, TokenType::PLUS, T0, 30);
    Instr T1Change(1, TokenType::PLUS, T1, 1);
    Jump J0(0);
    Label L2(2);
    RunCodeGenTest("void f() {"
                   "  int counter = 10;"
                   "  for (int i = 0; i < 20; ++i) {"
                   "    counter = counter + 30;"
                   "  }"
                   "  int result = counter;"
                   "}", {
      T0, T1, L0, If1, J2, L1, T0Change, T1Change, J0, L2
    });
  }
  SECTION(NestedFor) {
    Unary T0(0, 0);
    Label L0(0);
    If If1(TokenType::LT, T0, 10, 1);
    Jump J8(8);
    Label L1(1);
    Unary T1(1, 0);
    Label L2(2);
    If If3(TokenType::LT, T1, 20, 3);
    Jump J7(7);
    Label L3(3);
    Unary T2(2, 0);
    Label L4(4);
    If If5(TokenType::LT, T2, 30, 5);
    Jump J6(6);
    Label L5(5);
    Unary T3(3, 0);
    Instr T2Change(2, TokenType::PLUS, T2, 1);
    Jump J4(4);
    Label L6(6);
    Unary T5(5, 1);
    Instr T1Change(1, TokenType::PLUS, T1, 1);
    Jump J2(2);
    Label L7(7);
    Unary T7(7, 2);
    Instr T0Change(0, TokenType::PLUS, T0, 1);
    Jump J0(0);
    Label L8(8);
    Unary T9(9, 3);
    RunCodeGenTest("void f() {"
                   "  for (int i = 0; i < 10; ++i) {"
                   "    for (int j = 0; j < 20; ++j) {"
                   "      for (int k = 0; k < 30; ++k) {"
                   "        int result = 0;"
                   "      }"
                   "      int result = 1;"
                   "    }"
                   "    int result = 2;"
                   "  }"
                   "  int result = 3;"
                   "}", {
      T0, L0, If1, J8, L1, T1, L2, If3, J7, L3, T2, L4, If5, J6, L5, T3, T2Change, J4, L6,
      T5, T1Change, J2, L7, T7, T0Change, J0, L8, T9
    });
  }
  SECTION(Assignment) {
    RunCodeGenTest("void f() {"
                   "  int a = 1;"
                   "  a = 2;"
                   "  a = 3;"
                   "  int b = a;"
                   "  b = a;"
                   "  b = 5;"
                   "}", {
      Unary(0, 1),
      Unary(0, 2),
      Unary(0, 3),
      Unary(0, Unary(0, 3)),
      Unary(0, 5)
    });
  }
  SECTION(Assignment) {
    Unary T0(0, 1);
    Unary T1(1, 0);
    Instr T7(7, TokenType::PLUS, T0, T0);
    Ref R7(T7);
    RunCodeGenTest("void f() {"
                   "  int a = 1;"
                   "  int b = 0;"
                   "  a = 2;"
                   "  a = a;"
                   "  a = b;"
                   "  ++a;"
                   "  ++b;"
                   "  a = b + a + a;"
                   "}", {
      T0, T1,
      Unary(0, 2),
      Unary(0, T0),
      Unary(0, T1),
      Instr(0, TokenType::PLUS, T0, 1),
      Instr(1, TokenType::PLUS, T1, 1),
      Instr(7, TokenType::PLUS, T0, T0),
      Instr(0, TokenType::PLUS, T1, R7)
    });
  }
  SECTION(Variables) {
    Instr T0(0, TokenType::PLUS, 2, 1);
    Instr T1(1, TokenType::PLUS, 2, 1);
    Instr T2(2, TokenType::PLUS, T0, T1);
    Instr T3(3, TokenType::PLUS, 3, 4);
    Instr T4(4, TokenType::PLUS, T3, T2);
    Instr T5(5, TokenType::PLUS, T4, T2);
    Instr T6(6, TokenType::PLUS, T5, T2);
    Instr T7(7, TokenType::PLUS, T6, T2);
    RunCodeGenTest("void f() {"
                   "  int a = 2++ + ++2;"
                   "  int b = 3 + 4;"
                   "  int c = b + a;"
                   "  int d = c + a;"
                   "  int e = d + a;"
                   "  int f = e + a;"
                   "}", {
      T0, T1, T2, T3, T4, T5, T6, T7
    });
  }
  SECTION(ComplexTest) {
    Unary T0(0, 1);
    Unary T1(1, 1);
    Label L0(0);
    If If1(TokenType::LT, T0, 5, 1);
    Jump J7(7);
    Label L1(1);
    Instr T2(2, TokenType::STAR, T1, T0);
    Unary T3(3, 0);
    Label L2(2);
    If If3(TokenType::LT, T3, 10, 3);
    Jump J6(6);
    Label L3(3);
    Label L4(4);
    Instr T4(4, TokenType::PLUS, T2, T3);
    Instr T5(5, TokenType::PLUS, T1, T4);
    Instr T6(6, TokenType::PLUS, T0, T5);
    If If5(TokenType::LT, T0, T1, 5);
    Jump J4(4);
    Label L5(5);
    Instr T3Change(3, TokenType::PLUS, T3, 1);
    Jump J2(2);
    Label L6(6);
    Jump J0(0);
    Label L7(7);
    RunCodeGenTest("void f() {"
                   "  int a = 1;"
                   "  int b = 1;"
                   "  while (a < 5) {"
                   "    int c = b * a;"
                   "    for (int d = 0; d < 10; ++d) {"
                   "      do {"
                   "        int e = a + b + c + d;"
                   "      } while (a < b);"
                   "    }"
                   "  }"
                   "}", {
      T0, T1, L0, If1, J7, L1, T2, T3, L2, If3, J6, L3, L4, T4, T5, T6, If5,
      J4, L5, T3Change, J2, L6, J0, L7
    });
  }
  SECTION(DoWhileBreak) {
    Label L0(0);
    Instr T0(0, TokenType::PLUS, 1, 2);
    Jump J1(1);
    Instr T1(1, TokenType::PLUS, 3, 4);
    If If1(TokenType::NEQ, 1, 0, 1);
    Jump J0(0);
    Label L1(1);
    Instr T2(2, TokenType::PLUS, 5, 6);
    RunCodeGenTest("void f() {"
                   "  do {"
                   "    int a = 1 + 2;"
                   "    break;"
                   "    int b = 3 + 4;"
                   "  } while (1);"
                   "  int a = 5 + 6;"
                   "}", {
      L0, T0, J1, T1, If1, J0, L1, T2
    });
  }
  SECTION(DoWhileContinue) {
    Label L0(0);
    Instr T0(0, TokenType::PLUS, 1, 2);
    Jump J0(0);
    Instr T1(1, TokenType::PLUS, 3, 4);
    If If1(TokenType::NEQ, 1, 0, 1);
    Label L1(1);
    Instr T2(2, TokenType::PLUS, 5, 6);
    RunCodeGenTest("void f() {"
                   "  do {"
                   "    int a = 1 + 2;"
                   "    continue;"
                   "    int b = 3 + 4;"
                   "  } while (1);"
                   "  int a = 5 + 6;"
                   "}", {
      L0, T0, J0, T1, If1, J0, L1, T2
    });
  }
  SECTION(WhileBreak) {
    Label L0(0);
    If If1(TokenType::NEQ, 1, 0, 1);
    Jump J2(2);
    Label L1(1);
    Instr T0(0, TokenType::PLUS, 1, 2);
    // Jump J2(2);
    Instr T1(1, TokenType::PLUS, 3, 4);
    Jump J0(0);
    Label L2(2);
    Instr T2(2, TokenType::PLUS, 5, 6);
    RunCodeGenTest("void f() {"
                   "  while (1) {"
                   "    int a = 1 + 2;"
                   "    break;"
                   "    int b = 3 + 4;"
                   "  }"
                   "  int c = 5 + 6;"
                   "}", {
      L0, If1, J2, L1, T0, J2, T1, J0, L2, T2
    });
  }
  SECTION(WhileContinue) {
    Label L0(0);
    If If1(TokenType::NEQ, 1, 0, 1);
    Jump J2(2);
    Label L1(1);
    Instr T0(0, TokenType::PLUS, 1, 2);
    Instr T1(1, TokenType::PLUS, 3, 4);
    Jump J0(0);
    Label L2(2);
    Instr T2(2, TokenType::PLUS, 5, 6);
    RunCodeGenTest("void f() {"
                   "  while (1) {"
                   "    int a = 1 + 2;"
                   "    continue;"
                   "    int b = 3 + 4;"
                   "  }"
                   "  int c = 5 + 6;"
                   "}", {
      L0, If1, J2, L1, T0, J0, T1, J0, L2, T2
    });
  }
  SECTION(ForBreak) {
    Unary T0(0, 0);
    Label L0(0);
    If If1(TokenType::LT, T0, 10, 1);
    Jump J2(2);
    Label L1(1);
    Instr T1(1, TokenType::PLUS, 1, 2);
    // Jump J2(2);
    Instr T2(2, TokenType::PLUS, 3, 4);
    Instr T0Change(0, TokenType::PLUS, T0, 1);
    Jump J0(0);
    Label L2(2);
    Instr T4(4, TokenType::PLUS, 5, 6);
    RunCodeGenTest("void f() {"
                   "  for (int i = 0; i < 10; ++i) {"
                   "    int a = 1 + 2;"
                   "    break;"
                   "    int b = 3 + 4;"
                   "  }"
                   "  int c = 5 + 6;"
                   "}", {
      T0, L0, If1, J2, L1, T1, J2, T2, T0Change, J0, L2, T4
    });
  }
  SECTION(ForContinue) {
    Unary T0(0, 0);
    Label L0(0);
    If If1(TokenType::LT, T0, 10, 1);
    Jump J2(2);
    Label L1(1);
    Instr T1(1, TokenType::PLUS, 1, 2);
    Jump J0(0);
    Instr T2(2, TokenType::PLUS, 3, 4);
    Instr T0Change(0, TokenType::PLUS, T0, 1);
    // Jump J0(0);
    Label L2(2);
    Instr T4(4, TokenType::PLUS, 5, 6);
    RunCodeGenTest("void f() {"
                   "  for (int i = 0; i < 10; ++i) {"
                   "    int a = 1 + 2;"
                   "    continue;"
                   "    int b = 3 + 4;"
                   "  }"
                   "  int c = 5 + 6;"
                   "}", {
      T0, L0, If1, J2, L1, T1, J0, T2, T0Change, J0, L2, T4
    });
  }
}