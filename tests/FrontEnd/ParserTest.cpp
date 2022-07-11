#include "FrontEnd/Parse/Parser.hpp"
#include "FrontEnd/AST/ASTPrettyPrint.hpp"
#include "FrontEnd/Lex/Lexer.hpp"
#include "MiddleEnd/Symbols/Storage.hpp"
#include "../TestHelpers.hpp"
#include <sstream>
#include <iostream>

using namespace weak::frontEnd;
using namespace weak::middleEnd;

static Lexer CreateLexer(Storage *S, std::string_view Input) {
  Lexer Lex(S, Input.begin(), Input.end());
  return Lex;
}

static void TestAST(std::string_view String, std::string_view Expected) {
  Storage Storage;
  auto Tokens = CreateLexer(&Storage, String).Analyze();
  Parser Parse(&*Tokens.begin(), &*Tokens.end());
  std::ostringstream OutStream;
  ASTPrettyPrint(Parse.Parse(), OutStream);
  std::string Output = OutStream.str();
  std::cout << Output << std::endl;
  TEST_CASE(OutStream.str() == Expected);
}

int main() {
  SECTION(BasicFunction) {
    TestAST("int main(int argc, char argv) {\n"
            "  return 0;\n"
            "}\n",
            "CompoundStmt <line:0, col:0>\n"
            "  FunctionDecl <line:1, col:1>\n"
            "    FunctionRetType <line:1, col:1> <INT>\n"
            "    FunctionName <line:1, col:1> main\n"
            "    FunctionArgs <line:1, col:1>\n"
            "      VarDeclStmt <line:1, col:10> <INT> argc\n"
            "      VarDeclStmt <line:1, col:20> <CHAR> argv\n"
            "    FunctionBody <line:1, col:1>\n"
            "      CompoundStmt <line:1, col:31>\n"
            "        ReturnStmt <line:2, col:3>\n"
            "          IntegerLiteral <line:2, col:10> 0\n");
  }
  SECTION(VoidReturn) {
    TestAST("void f() {\n"
            "  return;\n"
            "}\n",
            "CompoundStmt <line:0, col:0>\n"
            "  FunctionDecl <line:1, col:1>\n"
            "    FunctionRetType <line:1, col:1> <VOID>\n"
            "    FunctionName <line:1, col:1> f\n"
            "    FunctionArgs <line:1, col:1>\n"
            "    FunctionBody <line:1, col:1>\n"
            "      CompoundStmt <line:1, col:10>\n"
            "        ReturnStmt <line:2, col:3>\n");
  }
  SECTION(NonVoidReturn) {
    TestAST("float f() {\n"
            "  return 0.0;\n"
            "}\n",
            "CompoundStmt <line:0, col:0>\n"
            "  FunctionDecl <line:1, col:1>\n"
            "    FunctionRetType <line:1, col:1> <FLOAT>\n"
            "    FunctionName <line:1, col:1> f\n"
            "    FunctionArgs <line:1, col:1>\n"
            "    FunctionBody <line:1, col:1>\n"
            "      CompoundStmt <line:1, col:11>\n"
            "        ReturnStmt <line:2, col:3>\n"
            "          FloatingPointLiteral <line:2, col:10> 0\n");
  }
  SECTION(EmptyFunctionCall) {
    TestAST("void f() {\n"
            "  empty_function_call();\n"
            "}\n",
            "CompoundStmt <line:0, col:0>\n"
            "  FunctionDecl <line:1, col:1>\n"
            "    FunctionRetType <line:1, col:1> <VOID>\n"
            "    FunctionName <line:1, col:1> f\n"
            "    FunctionArgs <line:1, col:1>\n"
            "    FunctionBody <line:1, col:1>\n"
            "      CompoundStmt <line:1, col:10>\n"
            "        FunctionCall <line:2, col:3> empty_function_call\n"
            "          FunctionArgs <line:2, col:3>\n");
  }
  SECTION(ReturnEmptyFunctionCall) {
    TestAST("int f() {\n"
            "  return call();\n"
            "}\n",
            "CompoundStmt <line:0, col:0>\n"
            "  FunctionDecl <line:1, col:1>\n"
            "    FunctionRetType <line:1, col:1> <INT>\n"
            "    FunctionName <line:1, col:1> f\n"
            "    FunctionArgs <line:1, col:1>\n"
            "    FunctionBody <line:1, col:1>\n"
            "      CompoundStmt <line:1, col:9>\n"
            "        ReturnStmt <line:2, col:3>\n"
            "          FunctionCall <line:2, col:10> call\n"
            "            FunctionArgs <line:2, col:10>\n");
  }
  SECTION(ReturnEmptyFunctionCall) {
      TestAST("int f() {\n"
              "  return call(1, 2, 3);\n"
              "}\n",
              "CompoundStmt <line:0, col:0>\n"
              "  FunctionDecl <line:1, col:1>\n"
              "    FunctionRetType <line:1, col:1> <INT>\n"
              "    FunctionName <line:1, col:1> f\n"
              "    FunctionArgs <line:1, col:1>\n"
              "    FunctionBody <line:1, col:1>\n"
              "      CompoundStmt <line:1, col:9>\n"
              "        ReturnStmt <line:2, col:3>\n"
              "          FunctionCall <line:2, col:10> call\n"
              "            FunctionArgs <line:2, col:10>\n"
              "              IntegerLiteral <line:2, col:15> 1\n"
              "              IntegerLiteral <line:2, col:18> 2\n"
              "              IntegerLiteral <line:2, col:21> 3\n");
  }
  SECTION(ParametrizedFunctionCall) {
    TestAST("void f() {\n"
            "  do_work(1, a + b * c << 3);\n"
            "}\n",
            "CompoundStmt <line:0, col:0>\n"
            "  FunctionDecl <line:1, col:1>\n"
            "    FunctionRetType <line:1, col:1> <VOID>\n"
            "    FunctionName <line:1, col:1> f\n"
            "    FunctionArgs <line:1, col:1>\n"
            "    FunctionBody <line:1, col:1>\n"
            "      CompoundStmt <line:1, col:10>\n"
            "        FunctionCall <line:2, col:3> do_work\n"
            "          FunctionArgs <line:2, col:3>\n"
            "            IntegerLiteral <line:2, col:11> 1\n"
            "            BinaryOperator <line:2, col:24> >>\n"
            "              BinaryOperator <line:2, col:16> +\n"
            "                Symbol <line:2, col:14> a\n"
            "                BinaryOperator <line:2, col:20> *\n"
            "                  Symbol <line:2, col:18> b\n"
            "                  Symbol <line:2, col:22> c\n"
            "              IntegerLiteral <line:2, col:27> 3\n");
  }
  SECTION(OperatorsPriority) {
    TestAST("void f() {\n"
            "  a << b + c = a << b + c = x <= e == f >= g <= e == f >= g;\n"
            "}\n",
            "CompoundStmt <line:0, col:0>\n"
            "  FunctionDecl <line:1, col:1>\n"
            "    FunctionRetType <line:1, col:1> <VOID>\n"
            "    FunctionName <line:1, col:1> f\n"
            "    FunctionArgs <line:1, col:1>\n"
            "    FunctionBody <line:1, col:1>\n"
            "      CompoundStmt <line:1, col:10>\n"
            "        BinaryOperator <line:2, col:14> =\n"
            "          BinaryOperator <line:2, col:5> >>\n"
            "            Symbol <line:2, col:3> a\n"
            "            BinaryOperator <line:2, col:10> +\n"
            "              Symbol <line:2, col:8> b\n"
            "              Symbol <line:2, col:12> c\n"
            "          BinaryOperator <line:2, col:27> =\n"
            "            BinaryOperator <line:2, col:18> >>\n"
            "              Symbol <line:2, col:16> a\n"
            "              BinaryOperator <line:2, col:23> +\n"
            "                Symbol <line:2, col:21> b\n"
            "                Symbol <line:2, col:25> c\n"
            "            BinaryOperator <line:2, col:36> ==\n"
            "              BinaryOperator <line:2, col:31> <=\n"
            "                Symbol <line:2, col:29> x\n"
            "                Symbol <line:2, col:34> e\n"
            "              BinaryOperator <line:2, col:51> ==\n"
            "                BinaryOperator <line:2, col:41> >=\n"
            "                  Symbol <line:2, col:39> f\n"
            "                  BinaryOperator <line:2, col:46> <=\n"
            "                    Symbol <line:2, col:44> g\n"
            "                    Symbol <line:2, col:49> e\n"
            "                BinaryOperator <line:2, col:56> >=\n"
            "                  Symbol <line:2, col:54> f\n"
            "                  Symbol <line:2, col:59> g\n");
  }
  SECTION(UnaryOperators) {
    TestAST("void f() {\n"
            "  int var1 = 0;\n"
            "  var1++;\n"
            "  var1--;\n"
            "  --var1;\n"
            "  ++var1;\n"
            "}\n",
            "CompoundStmt <line:0, col:0>\n"
            "  FunctionDecl <line:1, col:1>\n"
            "    FunctionRetType <line:1, col:1> <VOID>\n"
            "    FunctionName <line:1, col:1> f\n"
            "    FunctionArgs <line:1, col:1>\n"
            "    FunctionBody <line:1, col:1>\n"
            "      CompoundStmt <line:1, col:10>\n"
            "        VarDeclStmt <line:2, col:3> <INT> var1\n"
            "          IntegerLiteral <line:2, col:14> 0\n"
            "        Postfix UnaryOperator <line:3, col:7> ++\n"
            "          Symbol <line:3, col:3> var1\n"
            "        Postfix UnaryOperator <line:4, col:7> --\n"
            "          Symbol <line:4, col:3> var1\n"
            "        Prefix UnaryOperator <line:5, col:3> --\n"
            "          Symbol <line:5, col:5> var1\n"
            "        Prefix UnaryOperator <line:6, col:3> ++\n"
            "          Symbol <line:6, col:5> var1\n");
  }
  SECTION(Variables) {
    TestAST("void f() {\n"
            "  float value = 3.1415;\n"
            "  bool good_boolean = true;\n"
            "  bool malevolent_boolean = false;\n"
            "}\n",
            "CompoundStmt <line:0, col:0>\n"
            "  FunctionDecl <line:1, col:1>\n"
            "    FunctionRetType <line:1, col:1> <VOID>\n"
            "    FunctionName <line:1, col:1> f\n"
            "    FunctionArgs <line:1, col:1>\n"
            "    FunctionBody <line:1, col:1>\n"
            "      CompoundStmt <line:1, col:10>\n"
            "        VarDeclStmt <line:2, col:3> <FLOAT> value\n"
            "          FloatingPointLiteral <line:2, col:17> 3.1415\n"
            "        VarDeclStmt <line:3, col:3> <BOOLEAN> good_boolean\n"
            "          BooleanLiteral <line:3, col:23> true\n"
            "        VarDeclStmt <line:4, col:3> <BOOLEAN> malevolent_boolean\n"
            "          BooleanLiteral <line:4, col:29> false\n");
  }
  SECTION(If) {
    TestAST("void f() {\n"
            "  if (1) {\n"
            "    if (2) {\n"
            "      --a;\n"
            "    } else {\n"
            "      if (3) {\n"
            "        ++b;\n"
            "      }\n"
            "    }\n"
            "  } else {\n"
            "    if (4) {\n"
            "      ++a;\n"
            "    } else {\n"
            "      --b;\n"
            "    }\n"
            "  }\n"
            "}\n",
            "CompoundStmt <line:0, col:0>\n"
            "  FunctionDecl <line:1, col:1>\n"
            "    FunctionRetType <line:1, col:1> <VOID>\n"
            "    FunctionName <line:1, col:1> f\n"
            "    FunctionArgs <line:1, col:1>\n"
            "    FunctionBody <line:1, col:1>\n"
            "      CompoundStmt <line:1, col:10>\n"
            "        IfStmt <line:2, col:3>\n"
            "          IfStmtCondition <line:2, col:7>\n"
            "            IntegerLiteral <line:2, col:7> 1\n"
            "          IfStmtThenBody <line:2, col:10>\n"
            "            CompoundStmt <line:2, col:10>\n"
            "              IfStmt <line:3, col:5>\n"
            "                IfStmtCondition <line:3, col:9>\n"
            "                  IntegerLiteral <line:3, col:9> 2\n"
            "                IfStmtThenBody <line:3, col:12>\n"
            "                  CompoundStmt <line:3, col:12>\n"
            "                    Prefix UnaryOperator <line:4, col:7> --\n"
            "                      Symbol <line:4, col:9> a\n"
            "                IfStmtElseBody <line:5, col:12>\n"
            "                  CompoundStmt <line:5, col:12>\n"
            "                    IfStmt <line:6, col:7>\n"
            "                      IfStmtCondition <line:6, col:11>\n"
            "                        IntegerLiteral <line:6, col:11> 3\n"
            "                      IfStmtThenBody <line:6, col:14>\n"
            "                        CompoundStmt <line:6, col:14>\n"
            "                          Prefix UnaryOperator <line:7, col:9> ++\n"
            "                            Symbol <line:7, col:11> b\n"
            "          IfStmtElseBody <line:10, col:10>\n"
            "            CompoundStmt <line:10, col:10>\n"
            "              IfStmt <line:11, col:5>\n"
            "                IfStmtCondition <line:11, col:9>\n"
            "                  IntegerLiteral <line:11, col:9> 4\n"
            "                IfStmtThenBody <line:11, col:12>\n"
            "                  CompoundStmt <line:11, col:12>\n"
            "                    Prefix UnaryOperator <line:12, col:7> ++\n"
            "                      Symbol <line:12, col:9> a\n"
            "                IfStmtElseBody <line:13, col:12>\n"
            "                  CompoundStmt <line:13, col:12>\n"
            "                    Prefix UnaryOperator <line:14, col:7> --\n"
            "                      Symbol <line:14, col:9> b\n");
  }
  SECTION(For) {
    TestAST("void f() {\n"
            "  for (int i = 0; i < 10; i = i * 2) {\n"
            "    int result = i * 2;\n"
            "  }\n"
            "}\n",
            "CompoundStmt <line:0, col:0>\n"
            "  FunctionDecl <line:1, col:1>\n"
            "    FunctionRetType <line:1, col:1> <VOID>\n"
            "    FunctionName <line:1, col:1> f\n"
            "    FunctionArgs <line:1, col:1>\n"
            "    FunctionBody <line:1, col:1>\n"
            "      CompoundStmt <line:1, col:10>\n"
            "        ForStmt <line:2, col:3>\n"
            "          ForStmtInit <line:2, col:8>\n"
            "            VarDeclStmt <line:2, col:8> <INT> i\n"
            "              IntegerLiteral <line:2, col:16> 0\n"
            "          ForStmtCondition <line:2, col:21>\n"
            "            BinaryOperator <line:2, col:21> <\n"
            "              Symbol <line:2, col:19> i\n"
            "              IntegerLiteral <line:2, col:23> 10\n"
            "          ForStmtIncrement <line:2, col:29>\n"
            "            BinaryOperator <line:2, col:29> =\n"
            "              Symbol <line:2, col:27> i\n"
            "              BinaryOperator <line:2, col:33> *\n"
            "                Symbol <line:2, col:31> i\n"
            "                IntegerLiteral <line:2, col:35> 2\n"
            "          ForStmtBody <line:2, col:38>\n"
            "            CompoundStmt <line:2, col:38>\n"
            "              VarDeclStmt <line:3, col:5> <INT> result\n"
            "                BinaryOperator <line:3, col:20> *\n"
            "                  Symbol <line:3, col:18> i\n"
            "                  IntegerLiteral <line:3, col:22> 2\n");
  }
  SECTION(While) {
    TestAST("void f() {\n"
            "  while (a < b) {\n"
            "    --a;\n"
            "  }\n"
            "}\n",
            "CompoundStmt <line:0, col:0>\n"
            "  FunctionDecl <line:1, col:1>\n"
            "    FunctionRetType <line:1, col:1> <VOID>\n"
            "    FunctionName <line:1, col:1> f\n"
            "    FunctionArgs <line:1, col:1>\n"
            "    FunctionBody <line:1, col:1>\n"
            "      CompoundStmt <line:1, col:10>\n"
            "        WhileStmt <line:2, col:3>\n"
            "          WhileStmtCond <line:2, col:12>\n"
            "            BinaryOperator <line:2, col:12> <\n"
            "              Symbol <line:2, col:10> a\n"
            "              Symbol <line:2, col:14> b\n"
            "          WhileStmtBody <line:2, col:17>\n"
            "            CompoundStmt <line:2, col:17>\n"
            "              Prefix UnaryOperator <line:3, col:5> --\n"
            "                Symbol <line:3, col:7> a\n");
  }
  SECTION(LoopBreakContinue) {
    TestAST("void f() {\n"
            "  for (int i = 0; i < 10; ++i) {\n"
            "    break;\n"
            "    continue;\n"
            "  }\n"
            "  while (a < b) {\n"
            "    break;\n"
            "    continue;\n"
            "  }\n"
            "  do {\n"
            "    break;\n"
            "    continue;\n"
            "  } while (b > a);\n"
            "}\n",
            "CompoundStmt <line:0, col:0>\n"
            "  FunctionDecl <line:1, col:1>\n"
            "    FunctionRetType <line:1, col:1> <VOID>\n"
            "    FunctionName <line:1, col:1> f\n"
            "    FunctionArgs <line:1, col:1>\n"
            "    FunctionBody <line:1, col:1>\n"
            "      CompoundStmt <line:1, col:10>\n"
            "        ForStmt <line:2, col:3>\n"
            "          ForStmtInit <line:2, col:8>\n"
            "            VarDeclStmt <line:2, col:8> <INT> i\n"
            "              IntegerLiteral <line:2, col:16> 0\n"
            "          ForStmtCondition <line:2, col:21>\n"
            "            BinaryOperator <line:2, col:21> <\n"
            "              Symbol <line:2, col:19> i\n"
            "              IntegerLiteral <line:2, col:23> 10\n"
            "          ForStmtIncrement <line:2, col:27>\n"
            "            Prefix UnaryOperator <line:2, col:27> ++\n"
            "              Symbol <line:2, col:29> i\n"
            "          ForStmtBody <line:2, col:32>\n"
            "            CompoundStmt <line:2, col:32>\n"
            "              BreakStmt <line:3, col:5>\n"
            "              ContinueStmt <line:4, col:5>\n"
            "        WhileStmt <line:6, col:3>\n"
            "          WhileStmtCond <line:6, col:12>\n"
            "            BinaryOperator <line:6, col:12> <\n"
            "              Symbol <line:6, col:10> a\n"
            "              Symbol <line:6, col:14> b\n"
            "          WhileStmtBody <line:6, col:17>\n"
            "            CompoundStmt <line:6, col:17>\n"
            "              BreakStmt <line:7, col:5>\n"
            "              ContinueStmt <line:8, col:5>\n"
            "        DoWhileStmt <line:10, col:3>\n"
            "          DoWhileStmtBody <line:10, col:6>\n"
            "            CompoundStmt <line:10, col:6>\n"
            "              BreakStmt <line:11, col:5>\n"
            "              ContinueStmt <line:12, col:5>\n"
            "          DoWhileStmtCond <line:13, col:14>\n"
            "            BinaryOperator <line:13, col:14> >\n"
            "              Symbol <line:13, col:12> b\n"
            "              Symbol <line:13, col:16> a\n");
  }
}