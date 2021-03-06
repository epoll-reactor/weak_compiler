/* Token.hpp - List of all token types and token definition itself.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_FRONTEND_LEX_TOKEN_HPP
#define WEAK_COMPILER_FRONTEND_LEX_TOKEN_HPP

#include <string>

namespace weak {
namespace frontEnd {

enum struct TokenType {
  NONE,
  // Keywords.
  BOOLEAN,
  BREAK,
  CHAR,
  CONTINUE,
  DO,
  ELSE,
  FALSE,
  FLOAT,
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

const char *TokenToString(TokenType Type);

struct Token {
  Token(std::string_view TheData, TokenType TheType, unsigned TheLineNo,
        unsigned TheColumnNo, unsigned TheAttribute = 0U);

  bool operator==(const Token &rhs) const;

  bool operator!=(const Token &rhs) const;

  /// Data if any (digits, symbols, literals).
  std::string Data;

  /// Token type.
  TokenType Type;

  /// Position in source text.
  unsigned LineNo;

  /// Position in source text.
  unsigned ColumnNo;

  /// Integer value used to represent variable in symbol table.
  /// Used by CFG builder/analyzer and code generator.
  unsigned Attribute;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_LEX_TOKEN_HPP
