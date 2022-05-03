#include "FrontEnd/AST/ASTPrettyPrint.hpp"
#include "ASTMakeFunctions.hpp"
#include "TestHelpers.hpp"
#include <sstream>
#include <algorithm>

using weak::frontEnd::TokenType;

void ASTPrettyPrintToStdout(const std::unique_ptr<ASTNode> &AST) {
  ASTPrettyPrint(AST, std::cout);
}

int main() {
  SECTION(Binary) {
    ASTPrettyPrintToStdout(MakeBinary(
        TokenType::PLUS, MakeInteger(1),
        MakeBinary(
            TokenType::PLUS, MakeInteger(2),
            MakeBinary(
                TokenType::PLUS, MakeInteger(3),
                MakeBinary(TokenType::PLUS,
                           MakeUnary(ASTUnaryOperator::UnaryType::POSTFIX,
                                     TokenType::DEC, MakeFloat(123.123)),
                           MakeString("123"))))));

    ASTPrettyPrintToStdout(MakeUnary(ASTUnaryOperator::UnaryType::POSTFIX,
                             TokenType::INC, MakeString("Some text")));

    ASTPrettyPrintToStdout(MakeBinary(
        TokenType::PLUS_ASSIGN,
        MakeBinary(TokenType::PLUS_ASSIGN, MakeContinue(), MakeBreak()),
        MakeInteger(1)));
  }

  SECTION(IfStmt) {
    std::vector<std::unique_ptr<ASTNode>> IfBlockStmts;
    std::vector<std::unique_ptr<ASTNode>> ElseBlockStmts;

    for (signed It = 0; It < 5; ++It)
      IfBlockStmts.push_back(MakeInteger(It));
    for (signed It = 5; It >= 0; --It)
      ElseBlockStmts.push_back(MakeInteger(It));

    std::vector<std::unique_ptr<ASTNode>> IfBlockStmts2;
    std::vector<std::unique_ptr<ASTNode>> ElseBlockStmts2;

    for (signed It = 0; It < 5; ++It)
      IfBlockStmts2.push_back(MakeInteger(It));
    for (signed It = 5; It >= 0; --It)
      ElseBlockStmts2.push_back(MakeInteger(It));

    auto If = MakeIf(MakeInteger(0), MakeCompound(std::move(IfBlockStmts2)),
                     MakeCompound(std::move(ElseBlockStmts2)));

    ElseBlockStmts.push_back(std::move(If));
    ElseBlockStmts.push_back(MakeString("Хоп хей"));

    ASTPrettyPrintToStdout(MakeIf(MakeInteger(0), MakeCompound(std::move(IfBlockStmts)),
                          MakeCompound(std::move(ElseBlockStmts))));
  }

  SECTION(ForStmt) {
    std::vector<std::unique_ptr<ASTNode>> ForBlockStmts;

    for (signed It = 0; It < 5; ++It)
      ForBlockStmts.push_back(MakeInteger(It));

    ASTPrettyPrintToStdout(
        MakeFor(MakeInteger(1),
                MakeBinary(TokenType::NEQ, MakeInteger(1), MakeInteger(2)),
                MakeUnary(ASTUnaryOperator::UnaryType::POSTFIX, TokenType::INC,
                          MakeString("123")),
                MakeCompound(std::move(ForBlockStmts))));
  }

  SECTION(WhileStmt) {
    std::vector<std::unique_ptr<ASTNode>> WhileBlockStmts;

    for (signed It = 0; It < 5; ++It)
      WhileBlockStmts.push_back(MakeInteger(It));

    ASTPrettyPrintToStdout(
        MakeWhile(MakeInteger(1), MakeCompound(std::move(WhileBlockStmts))));
  }

  SECTION(DoWhileStmt) {
    std::vector<std::unique_ptr<ASTNode>> WhileBlockStmts;

    for (signed It = 0; It < 5; ++It)
      WhileBlockStmts.push_back(MakeInteger(It));

    ASTPrettyPrintToStdout(
        MakeDoWhile(MakeCompound(std::move(WhileBlockStmts)), MakeInteger(1)));
  }

  SECTION(ReturnStmt) {
    ASTPrettyPrintToStdout(MakeReturn(
        MakeBinary(TokenType::PLUS, MakeInteger(120), MakeInteger(120))));
  }

  SECTION(VarDeclStmt) {
    ASTPrettyPrintToStdout(MakeVarDecl(
        TokenType::STRING, "Variable_1",
        MakeBinary(TokenType::PLUS, MakeString("First"),
                   MakeVarDecl(TokenType::STRING, "Variable_2",
                               MakeBinary(TokenType::PLUS, MakeString("Second"),
                                          MakeString("Third"))))));
  }

  SECTION(FunctionDecl) {
    std::vector<std::unique_ptr<ASTNode>> Arguments, Body;
    Arguments.push_back(MakeString("Arg"));
    Body.push_back(
        MakeBinary(TokenType::PLUS_ASSIGN, MakeInteger(10), MakeInteger(20)));

    ASTPrettyPrintToStdout(MakeFunction(TokenType::VOID, "FunctionName",
                                std::move(Arguments),
                                MakeCompound(std::move(Body))));
  }

  SECTION(FunctionCall) {
    std::vector<std::unique_ptr<ASTNode>> Arguments;
    Arguments.push_back(MakeString("Arg"));
    Arguments.push_back(MakeString("Arg"));
    Arguments.push_back(MakeString("Arg"));
    Arguments.push_back(MakeString("Arg"));
    std::ostringstream Stream;
    ASTPrettyPrint(MakeFunctionCall("Fun", std::move(Arguments)), Stream);
  }
}
