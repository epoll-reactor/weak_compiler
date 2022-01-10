#include "FrontEnd/Lex/Lexer.hpp"
#include "FrontEnd/Symbols/Storage.hpp"
#include "Utility/Diagnostic.hpp"
#include <cassert>
#include <unordered_map>

using TokenType = weak::frontEnd::TokenType;

static const std::unordered_map<std::string_view, TokenType> LexKeywords = {
    {"bool", TokenType::BOOLEAN},  {"break", TokenType::BREAK},
    {"char", TokenType::CHAR},     {"continue", TokenType::CONTINUE},
    {"do", TokenType::DO},         {"else", TokenType::ELSE},
    {"false", TokenType::FALSE},   {"for", TokenType::FOR},
    {"if", TokenType::IF},         {"int", TokenType::INT},
    {"return", TokenType::RETURN}, {"string", TokenType::STRING},
    {"true", TokenType::TRUE},     {"void", TokenType::VOID},
    {"while", TokenType::WHILE}};

static const std::unordered_map<std::string_view, TokenType> LexOperators = {
    {"=", TokenType::ASSIGN},
    {"*=", TokenType::MUL_ASSIGN},
    {"/=", TokenType::DIV_ASSIGN},
    {"%=", TokenType::MOD_ASSIGN},
    {"+=", TokenType::PLUS_ASSIGN},
    {"-=", TokenType::MINUS_ASSIGN},
    {"<<=", TokenType::SHL_ASSIGN},
    {">>=", TokenType::SHR_ASSIGN},
    {"&=", TokenType::BIT_AND_ASSIGN},
    {"|=", TokenType::BIT_OR_ASSIGN},
    {"^=", TokenType::XOR_ASSIGN},
    {"&&", TokenType::AND},
    {"||", TokenType::OR},
    {"^", TokenType::XOR},
    {"&", TokenType::BIT_AND},
    {"|", TokenType::BIT_OR},
    {"==", TokenType::EQ},
    {"!=", TokenType::NEQ},
    {">", TokenType::GT},
    {"<", TokenType::LT},
    {">=", TokenType::GE},
    {"<=", TokenType::LE},
    {">>", TokenType::SHR},
    {"<<", TokenType::SHL},
    {"+", TokenType::PLUS},
    {"-", TokenType::MINUS},
    {"*", TokenType::STAR},
    {"/", TokenType::SLASH},
    {"%", TokenType::MOD},
    {"++", TokenType::INC},
    {"--", TokenType::DEC},
    {",", TokenType::COMMA},
    {";", TokenType::SEMICOLON},
    {"!", TokenType::NOT},
    {"[", TokenType::OPEN_BOX_BRACKET},
    {"]", TokenType::CLOSE_BOX_BRACKET},
    {"{", TokenType::OPEN_CURLY_BRACKET},
    {"}", TokenType::CLOSE_CURLY_BRACKET},
    {"(", TokenType::OPEN_PAREN},
    {")", TokenType::CLOSE_PAREN}};

namespace {

class LexStringLiteralCheck {
public:
  explicit LexStringLiteralCheck(char ThePeek) : Peek(ThePeek) {}

  void ClosingQuoteCheck(unsigned LineNo, unsigned ColumnNo) const {
    if (Peek == '\n' || Peek == '\0')
      weak::DiagnosticError(LineNo, ColumnNo) << "Closing \" expected";
  }

private:
  char Peek;
};

class LexDigitCheck {
public:
  LexDigitCheck(const std::string &TheDigit, char ThePeek, unsigned Dots)
      : Digit(TheDigit), Peek(ThePeek), DotsReached(Dots) {}

  void LastDigitRequire(unsigned LineNo, unsigned ColumnNo) const {
    if (std::isalpha(Peek) || !std::isdigit(Digit.back()))
      weak::DiagnosticError(LineNo, ColumnNo)
          << "Digit as last character expected";
  }

  void ExactOneDotRequire(unsigned LineNo, unsigned ColumnNo) const {
    if (DotsReached > 1)
      weak::DiagnosticError(LineNo, ColumnNo) << "Extra \".\" in digit";
  }

private:
  const std::string &Digit;
  char Peek;
  unsigned DotsReached;
};

} // namespace

static bool IsAlphanumeric(char C) { return isalpha(C) || C == '_'; }

static void NormalizeColumnPosition(std::string_view Data,
                                    weak::frontEnd::TokenType Type,
                                    unsigned &ColumnNo) {
  using weak::frontEnd::TokenType;
  using namespace std::string_view_literals;
  std::unordered_map<weak::frontEnd::TokenType, int> TokenLengths = {
      {TokenType::BOOLEAN, "bool"sv.length()},
      {TokenType::BREAK, "break"sv.length()},
      {TokenType::CHAR, "char"sv.length()},
      {TokenType::CONTINUE, "continue"sv.length()},
      {TokenType::DO, "do"sv.length()},
      {TokenType::ELSE, "else"sv.length()},
      {TokenType::FALSE, "false"sv.length()},
      {TokenType::FOR, "for"sv.length()},
      {TokenType::IF, "if"sv.length()},
      {TokenType::INT, "int"sv.length()},
      {TokenType::RETURN, "return"sv.length()},
      {TokenType::STRING, "string"sv.length()},
      {TokenType::TRUE, "true"sv.length()},
      {TokenType::VOID, "void"sv.length()},
      {TokenType::WHILE, "while"sv.length()},

      {TokenType::INTEGRAL_LITERAL, Data.length()},
      {TokenType::FLOATING_POINT_LITERAL, Data.length()},
      {TokenType::STRING_LITERAL, Data.length() + 2 /* quotes */},
      {TokenType::SYMBOL, Data.length()}};

  ColumnNo -= TokenLengths.at(Type);
}

namespace weak {
namespace frontEnd {

Lexer::Lexer(weak::frontEnd::Storage *TheStorage, const char *TheBufferStart,
             const char *TheBufferEnd)
    : Storage(TheStorage), BufferStart(TheBufferStart), BufferEnd(TheBufferEnd),
      CurrentBufferPtr(TheBufferStart), CurrentLineNo(0U), CurrentColumnNo(0U) {
  assert(BufferStart);
  assert(BufferEnd);
  assert(BufferStart <= BufferEnd);
}

std::vector<Token> Lexer::Analyze() {
  std::vector<Token> ProcessedTokens;

  while (CurrentBufferPtr != BufferEnd) {
    if (char Atom = PeekCurrent(); std::isdigit(Atom)) {
      ProcessedTokens.push_back(AnalyzeDigit());
    } else if (std::isalpha(Atom)) {
      ProcessedTokens.push_back(AnalyzeSymbol());
    } else if (Atom == '\"') {
      ProcessedTokens.push_back(AnalyzeStringLiteral());
    } else if (std::isspace(Atom)) {
      PeekNext();
      continue;
    } else {
      ProcessedTokens.push_back(AnalyzeOperator());
    }
  }

  return ProcessedTokens;
}

Token Lexer::AnalyzeDigit() {
  std::string Digit;
  bool DotErrorOccurred = false;
  unsigned DotErrorColumn = 0U;
  unsigned DotsReached = 0U;

  while (std::isdigit(PeekCurrent()) || PeekCurrent() == '.') {
    if (PeekCurrent() == '.')
      ++DotsReached;
    if (DotsReached > 1) {
      DotErrorOccurred = true;
      DotErrorColumn = CurrentColumnNo;
      break;
    }
    Digit += PeekNext();
  }

  unsigned LexColumnName = DotErrorOccurred ? DotErrorColumn : CurrentColumnNo;

  LexDigitCheck Checker(Digit, PeekCurrent(), DotsReached);
  Checker.LastDigitRequire(CurrentLineNo, LexColumnName);
  Checker.ExactOneDotRequire(CurrentLineNo, LexColumnName);

  return MakeToken(Digit, DotsReached == 0U
                              ? TokenType::INTEGRAL_LITERAL
                              : TokenType::FLOATING_POINT_LITERAL);
}

Token Lexer::AnalyzeStringLiteral() {
  PeekNext(); // Eat "

  if (PeekNext() == '\"') {
    return MakeToken("", TokenType::STRING_LITERAL);
  }
  --CurrentBufferPtr;

  std::string Literal;
  while (PeekCurrent() != '\"') {
    Literal += PeekNext();

    LexStringLiteralCheck Check(PeekCurrent());
    Check.ClosingQuoteCheck(CurrentLineNo, CurrentColumnNo);

    if (Literal.back() == '\\')
      Literal.back() = PeekNext();
  }
  assert(PeekCurrent() == '\"');

  PeekNext(); // Eat "
  --CurrentColumnNo;

  return MakeToken(Literal, TokenType::STRING_LITERAL);
}

Token Lexer::AnalyzeSymbol() {
  std::string Symbol;

  while ((IsAlphanumeric(PeekCurrent()) || std::isdigit(PeekCurrent())))
    Symbol += PeekNext();

  if (LexKeywords.find(Symbol) != LexKeywords.end())
    return MakeToken("", LexKeywords.at(Symbol));

  unsigned Attribute = Storage->AddSymbol(Symbol);

  unsigned LineNo = CurrentLineNo + 1;
  unsigned ColumnNo = CurrentColumnNo + 1;

  NormalizeColumnPosition(Symbol, TokenType::SYMBOL, ColumnNo);
  Token T(std::move(Symbol), TokenType::SYMBOL, LineNo, ColumnNo, Attribute);
  return T;
}

Token Lexer::AnalyzeOperator() {
  std::string Operator(1, PeekNext());
  unsigned SavedColumnNo = 0U;

  while (LexOperators.find(Operator) != LexOperators.end()) {
    char Next = *CurrentBufferPtr++;
    SavedColumnNo = CurrentColumnNo;
    if (Next == '\n') {
      CurrentColumnNo = 0U;
      CurrentLineNo++;
    }
    ++CurrentColumnNo;
    Operator += Next;

    if (LexOperators.find(Operator) == LexOperators.end()) {
      Operator.pop_back();
      --CurrentBufferPtr;

      if (CurrentColumnNo > 0U)
        --CurrentColumnNo;

      if (PeekCurrent() == '\n')
        --CurrentLineNo;

      break;
    }
  }

  if (LexOperators.find(Operator) != LexOperators.end()) {
    return Token("", LexOperators.at(Operator), CurrentLineNo + 1,
                 SavedColumnNo - Operator.length() + 1);
  }

  --CurrentColumnNo;
  DiagnosticError(CurrentLineNo, CurrentColumnNo)
      << "Unknown character sequence " << Operator;
  UnreachablePoint();
}

char Lexer::PeekNext() {
  char Atom = *CurrentBufferPtr++;
  if (Atom == '\n') {
    CurrentLineNo++;
    CurrentColumnNo = 0U;
  } else {
    CurrentColumnNo++;
  }
  return Atom;
}

char Lexer::PeekCurrent() const { return *CurrentBufferPtr; }

Token Lexer::MakeToken(std::string_view Data, TokenType Type) const {
  unsigned LineNo = CurrentLineNo + 1;
  unsigned ColumnNo = CurrentColumnNo + 1;

  NormalizeColumnPosition(Data, Type, ColumnNo);
  Token T(Data, Type, LineNo, ColumnNo);

  return T;
}

} // namespace frontEnd
} // namespace weak