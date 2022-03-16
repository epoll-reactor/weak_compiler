#include "FrontEnd/Lex/Lexer.hpp"
#include "MiddleEnd/Symbols/Storage.hpp"
#include "TestHelpers.hpp"

using namespace weak::frontEnd;
using namespace weak::middleEnd;

static Lexer CreateLexer(Storage *S, std::string_view Input) {
  Lexer Lex(S, Input.begin(), Input.end());
  return Lex;
}

static Token MakeToken(std::string_view Data, TokenType Type) {
  return {Data, Type, 0U, 0U};
}

static void RunLexerTest(std::string_view Input,
                         const std::vector<Token> &ExpectedTokens) {
  Storage S;
  auto Tokens = CreateLexer(&S, Input).Analyze();
  if (Tokens.size() != ExpectedTokens.size()) {
    std::cerr << "Output size mismatch: got " << Tokens.size()
              << " but expected " << ExpectedTokens.size();
    exit(-1);
  }
  for (size_t I = 0; I < Tokens.size(); ++I) {
    std::cout << "Token at line " << Tokens[I].LineNo << ", column "
              << Tokens[I].ColumnNo << ": " << TokenToString(Tokens[I].Type)
              << std::endl;
    if (Tokens[I] != ExpectedTokens[I]) {
      std::cerr << "got " << Tokens[I].Data << ", but expected "
                << ExpectedTokens[I].Data;
      exit(-1);
    }
  }
}

int main() {
  SECTION(LexingIntegralConstant) {
    std::vector<Token> Assertion = {
        MakeToken("1", TokenType::INTEGRAL_LITERAL),
        MakeToken("22", TokenType::INTEGRAL_LITERAL),
        MakeToken("333", TokenType::INTEGRAL_LITERAL)};
    RunLexerTest("1 22 333", Assertion);
  }
  SECTION(LexingFloatingPointConstant) {
    std::vector<Token> Assertion = {
        MakeToken("1.1", TokenType::FLOATING_POINT_LITERAL),
        MakeToken("22.22", TokenType::FLOATING_POINT_LITERAL),
        MakeToken("333.333", TokenType::FLOATING_POINT_LITERAL)};
    RunLexerTest("1.1 22.22 333.333", Assertion);
  }
  SECTION(LexingEmptyStringLiteral) {
    std::vector<Token> Assertion = {MakeToken("", TokenType::STRING_LITERAL)};
    RunLexerTest("\"\"", Assertion);
  }
  SECTION(LexingStringLiteral) {
    std::vector<Token> Assertion = {MakeToken("a", TokenType::STRING_LITERAL),
                                    MakeToken("b", TokenType::STRING_LITERAL),
                                    MakeToken("c", TokenType::STRING_LITERAL)};
    RunLexerTest(R"("a" "b" "c")", Assertion);
  }
  SECTION(LexingStringLiteral) {
    std::vector<Token> Assertion = {MakeToken("text \" with escaped character ",
                                              TokenType::STRING_LITERAL)};
    RunLexerTest(R"("text \" with escaped character ")", Assertion);
  }
  SECTION(LexingEscapeSequenceInStringLiteral) {
    std::vector<Token> Assertion = {
        MakeToken("\\escaped\\", TokenType::STRING_LITERAL)};
    RunLexerTest(R"("\\escaped\\")", Assertion);
  }
  SECTION(LexingSymbols) {
    std::vector<Token> Assertion = {MakeToken("a", TokenType::SYMBOL),
                                    MakeToken("b", TokenType::SYMBOL),
                                    MakeToken("c", TokenType::SYMBOL)};
    RunLexerTest("a b c", Assertion);
  }
  SECTION(LexingKeywords) {
    std::vector<Token> Assertion = {MakeToken("", TokenType::BOOLEAN),
                                    MakeToken("", TokenType::CHAR),
                                    MakeToken("", TokenType::WHILE)};
    RunLexerTest("bool\nchar\nwhile", Assertion);
  }
  SECTION(LexingOperators) {
    std::vector<Token> Assertion_1 = {MakeToken("", TokenType::PLUS),
                                      MakeToken("", TokenType::MINUS),
                                      MakeToken("", TokenType::SLASH)};
    RunLexerTest("+-/", Assertion_1);
    std::vector<Token> Assertion_2 = {
        MakeToken("", TokenType::INC), MakeToken("", TokenType::INC),
        MakeToken("", TokenType::INC), MakeToken("", TokenType::PLUS)};
    RunLexerTest("+++++++", Assertion_2);
  }
  SECTION(LexingCompoundInput) {
    std::vector<Token> Assertion = {
        MakeToken("", TokenType::VOID),
        MakeToken("main", TokenType::SYMBOL),
        MakeToken("", TokenType::OPEN_PAREN),
        MakeToken("", TokenType::INT),
        MakeToken("argc", TokenType::SYMBOL),
        MakeToken("", TokenType::COMMA),
        MakeToken("", TokenType::CHAR),
        MakeToken("argv", TokenType::SYMBOL),
        MakeToken("", TokenType::CLOSE_PAREN),

        MakeToken("", TokenType::OPEN_CURLY_BRACKET),
        MakeToken("", TokenType::STRING),
        MakeToken("output", TokenType::SYMBOL),
        MakeToken("", TokenType::ASSIGN),
        MakeToken("", TokenType::STRING_LITERAL),
        MakeToken("", TokenType::SEMICOLON),

        MakeToken("", TokenType::WHILE),
        MakeToken("", TokenType::OPEN_PAREN),
        MakeToken("1", TokenType::INTEGRAL_LITERAL),
        MakeToken("", TokenType::NEQ),
        MakeToken("0", TokenType::INTEGRAL_LITERAL),
        MakeToken("", TokenType::CLOSE_PAREN),

        MakeToken("", TokenType::OPEN_CURLY_BRACKET),

        MakeToken("output", TokenType::SYMBOL),
        MakeToken("", TokenType::PLUS_ASSIGN),
        MakeToken("Oder ist dieser Lastwagen vielleicht besser auf den blitzen "
                  "Zweiundzwanzigzöllner?",
                  TokenType::STRING_LITERAL),
        MakeToken("", TokenType::SEMICOLON),

        MakeToken("", TokenType::CLOSE_CURLY_BRACKET),
        MakeToken("", TokenType::CLOSE_CURLY_BRACKET)};
    RunLexerTest(R"__(void main(int argc, char argv) {
        string output = "";
        while (1 != 0) {
          output += "Oder ist dieser Lastwagen vielleicht besser auf den blitzen Zweiundzwanzigzöllner?";
        }
      }
    )__",
                 Assertion);
  }
  SECTION(LexerSpeedTest) {
    Storage S;
    std::string Body = "1.1 1.1 1.1 1.1 1.1 1.1 1.1 1.1 1.1 1.1 1.1 1.1 1.1 1.1"
                       "+++++++++++++++++++++++++++++++++++++++++++++++++++++++"
                       "\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\""
                       "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa ";
    for (size_t It = 0; It < 16; ++It)
      Body += std::string(Body);
    printf("Body size: %zu\n", Body.size());
    CreateLexer(&S, Body).Analyze();
  }
}
