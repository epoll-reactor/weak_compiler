#include "FrontEnd/Lex/Lexer.hpp"
#include "../TestHelpers.hpp"
#include "Utility/Diagnostic.hpp"

weak::frontEnd::Lexer CreateLexer(std::string_view Input) {
  weak::frontEnd::Lexer Lex(Input.begin(), Input.end());
  return Lex;
}

void RunLexerTest(std::string_view Input,
                  const std::vector<weak::frontEnd::Token> &ExpectedTokens) {
  auto Tokens = CreateLexer(Input).Analyze();
  if (Tokens.size() != ExpectedTokens.size()) {
    std::cerr << "Output size mismatch: got " << Tokens.size()
              << " but expected " << ExpectedTokens.size();
    exit(-1);
  }
  for (size_t I = 0; I < Tokens.size(); ++I) {
    if (Tokens[I] != ExpectedTokens[I]) {
      std::cerr << "got " << Tokens[I].Data << ", but expected "
                << ExpectedTokens[I].Data;
      exit(-1);
    }
  }
}

int main() {
  using weak::frontEnd::TokenType;
  SECTION(LexingIntegralConstant) {
    std::vector<weak::frontEnd::Token> Assertion = {
        {"1", TokenType::INTEGRAL_LITERAL},
        {"22", TokenType::INTEGRAL_LITERAL},
        {"333", TokenType::INTEGRAL_LITERAL}};
    RunLexerTest("1 22 333", Assertion);
  }
  SECTION(LexingFloatingPointConstant) {
    std::vector<weak::frontEnd::Token> Assertion = {
        {"1.1", TokenType::FLOATING_POINT_LITERAL},
        {"22.22", TokenType::FLOATING_POINT_LITERAL},
        {"333.333", TokenType::FLOATING_POINT_LITERAL}};
    RunLexerTest("1.1 22.22 333.333", Assertion);
  }
  SECTION(LexingEmptyStringLiteral) {
    std::vector<weak::frontEnd::Token> Assertion = {
        {"", TokenType::STRING_LITERAL}};
    RunLexerTest("\"\"", Assertion);
  }
  SECTION(LexingStringLiteral) {
    std::vector<weak::frontEnd::Token> Assertion = {
        {"a", TokenType::STRING_LITERAL},
        {"b", TokenType::STRING_LITERAL},
        {"c", TokenType::STRING_LITERAL}};
    RunLexerTest("\"a\" \"b\" \"c\"", Assertion);
  }
  SECTION(LexingStringLiteral) {
    std::vector<weak::frontEnd::Token> Assertion = {
        {"text \" with escaped character ", TokenType::STRING_LITERAL}};
    RunLexerTest(R"("text \" with escaped character ")", Assertion);
  }
  SECTION(LexingEscapeSequenceInStringLiteral) {
    std::vector<weak::frontEnd::Token> Assertion = {
        {"\\escaped\\", TokenType::STRING_LITERAL}};
    RunLexerTest(R"("\\escaped\\")", Assertion);
  }
  SECTION(LexingSymbols) {
    std::vector<weak::frontEnd::Token> Assertion = {{"a", TokenType::SYMBOL},
                                                    {"b", TokenType::SYMBOL},
                                                    {"c", TokenType::SYMBOL}};
    RunLexerTest("a b c", Assertion);
  }
  SECTION(LexingKeywords) {
    std::vector<weak::frontEnd::Token> Assertion = {{"", TokenType::BOOLEAN},
                                                    {"", TokenType::CHAR},
                                                    {"", TokenType::WHILE}};
    RunLexerTest("bool char while", Assertion);
  }
  SECTION(LexingOperators) {
    std::vector<weak::frontEnd::Token> Assertion_1 = {
        {"", TokenType::PLUS}, {"", TokenType::MINUS}, {"", TokenType::SLASH}};
    RunLexerTest("+-/", Assertion_1);
    std::vector<weak::frontEnd::Token> Assertion_2 = {{"", TokenType::INC},
                                                      {"", TokenType::INC},
                                                      {"", TokenType::INC},
                                                      {"", TokenType::PLUS}};
    RunLexerTest("+++++++", Assertion_2);
  }
  SECTION(LexingCompoundInput) {
    std::vector<weak::frontEnd::Token> Assertion = {
        {"", TokenType::VOID},
        {"main", TokenType::SYMBOL},
        {"", TokenType::OPEN_PAREN},
        {"", TokenType::INT},
        {"argc", TokenType::SYMBOL},
        {"", TokenType::COMMA},
        {"", TokenType::CHAR},
        {"argv", TokenType::SYMBOL},
        {"", TokenType::CLOSE_PAREN},

        {"", TokenType::OPEN_CURLY_BRACKET},
        {"", TokenType::STRING},
        {"output", TokenType::SYMBOL},
        {"", TokenType::ASSIGN},
        {"", TokenType::STRING_LITERAL},
        {"", TokenType::SEMICOLON},

        {"", TokenType::WHILE},
        {"", TokenType::OPEN_PAREN},
        {"1", TokenType::INTEGRAL_LITERAL},
        {"", TokenType::NEQ},
        {"0", TokenType::INTEGRAL_LITERAL},
        {"", TokenType::CLOSE_PAREN},

        {"", TokenType::OPEN_CURLY_BRACKET},

        {"output", TokenType::SYMBOL},
        {"", TokenType::PLUS_ASSIGN},
        {"stackoverflow, huh", TokenType::STRING_LITERAL},
        {"", TokenType::SEMICOLON},

        {"", TokenType::CLOSE_CURLY_BRACKET},
        {"", TokenType::CLOSE_CURLY_BRACKET},
    };
    RunLexerTest(R"__(
      void main(int argc, char argv) {
        string output = "";
        while (1 != 0) {
          output += "stackoverflow, huh";
        }
      }
    )__",
                 Assertion);
  }
}
