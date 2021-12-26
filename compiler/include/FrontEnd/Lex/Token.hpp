#ifndef WEAK_COMPILER_LEX_TOKEN_HPP
#define WEAK_COMPILER_LEX_TOKEN_HPP

#include <string>

namespace weak {
namespace frontEnd {

enum struct TokenType {
  // Keywords.
  BOOLEAN,
  BREAK,
  CHAR,
  CONTINUE,
  DO,
  FALSE,
  FOR,
  IF,
  INT,
  RETURN,
  STRING,
  TRUE,
  VOID,
  WHILE,

  // Literals.
  INTEGRAL_LITERAL,
  FLOATING_POINT_LITERAL,
  STRING_LITERAL,
  SYMBOL,

  // Operators.
  ASSIGN,              // =
  MUL_ASSIGN,          // *=
  DIV_ASSIGN,          // /=
  MOD_ASSIGN,          // %=
  PLUS_ASSIGN,         // +=
  MINUS_ASSIGN,        // -=
  SHL_ASSIGN,          // <<=
  SHR_ASSIGN,          // >>=
  BIT_AND_ASSIGN,      // &=
  BIT_OR_ASSIGN,       // |=
  XOR_ASSIGN,          // ^=
  AND,                 // &&
  OR,                  // ||
  XOR,                 // ^
  BIT_AND,             // &
  BIT_OR,              // |
  EQ,                  // ==
  NEQ,                 // !=
  GT,                  // >
  LT,                  // <
  GE,                  // >=
  LE,                  // <=
  SHL,                 // <<
  SHR,                 // >>
  PLUS,                // +
  MINUS,               // -
  STAR,                // *
  SLASH,               // /
  MOD,                 // %
  INC,                 // ++
  DEC,                 // --
  COMMA,               // ,
  SEMICOLON,           // ;
  NOT,                 // !
  OPEN_BOX_BRACKET,    // [
  CLOSE_BOX_BRACKET,   // ]
  OPEN_CURLY_BRACKET,  // {
  CLOSE_CURLY_BRACKET, // }
  OPEN_PAREN,          // (
  CLOSE_PAREN          // )
};

struct Token {
  std::string Data;
  TokenType Type;

  Token(std::string_view TheData, TokenType TheType)
      : Data(TheData), Type(TheType) {}

  bool operator==(const Token &rhs) const {
    return Data == rhs.Data && Type == rhs.Type;
  }
  bool operator!=(const Token &rhs) const { return !(rhs == *this); }
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_LEX_TOKEN_HPP
