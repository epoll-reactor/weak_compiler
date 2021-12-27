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

const char *TokenToString(TokenType Type) {
  switch (Type) {
  case TokenType::BOOLEAN:
    return "<BOOLEAN>";
  case TokenType::BREAK:
    return "<BREAK>";
  case TokenType::CHAR:
    return "<CHAR>";
  case TokenType::CONTINUE:
    return "<CONTINUE>";
  case TokenType::DO:
    return "<DO>";
  case TokenType::FALSE:
    return "<FALSE>";
  case TokenType::FOR:
    return "<FOR>";
  case TokenType::IF:
    return "<IF>";
  case TokenType::INT:
    return "<INT>";
  case TokenType::RETURN:
    return "<RETURN>";
  case TokenType::STRING:
    return "<STRING>";
  case TokenType::TRUE:
    return "<TRUE>";
  case TokenType::VOID:
    return "<VOID>";
  case TokenType::WHILE:
    return "<WHILE>";
  case TokenType::INTEGRAL_LITERAL:
    return "<INT LITERAL>";
  case TokenType::FLOATING_POINT_LITERAL:
    return "<FLOAT LITERAL>";
  case TokenType::STRING_LITERAL:
    return "STRING LITERAL";
  case TokenType::SYMBOL:
    return "<SYMBOL>";
  case TokenType::ASSIGN:
    return "=";
  case TokenType::MUL_ASSIGN:
    return "*=";
  case TokenType::DIV_ASSIGN:
    return "/=";
  case TokenType::MOD_ASSIGN:
    return "%=";
  case TokenType::PLUS_ASSIGN:
    return "+=";
  case TokenType::MINUS_ASSIGN:
    return "-=";
  case TokenType::SHL_ASSIGN:
    return "<<=";
  case TokenType::SHR_ASSIGN:
    return ">>=";
  case TokenType::BIT_AND_ASSIGN:
    return "&=";
  case TokenType::BIT_OR_ASSIGN:
    return "|=";
  case TokenType::XOR_ASSIGN:
    return "^=";
  case TokenType::AND:
    return "&&";
  case TokenType::OR:
    return "||";
  case TokenType::XOR:
    return "^";
  case TokenType::BIT_AND:
    return "&";
  case TokenType::BIT_OR:
    return "|";
  case TokenType::EQ:
    return "==";
  case TokenType::NEQ:
    return "!=";
  case TokenType::GT:
    return ">";
  case TokenType::LT:
    return "<";
  case TokenType::GE:
    return ">=";
  case TokenType::LE:
    return "<=";
  case TokenType::SHL:
    return ">>";
  case TokenType::SHR:
    return "<<";
  case TokenType::PLUS:
    return "+";
  case TokenType::MINUS:
    return "-";
  case TokenType::STAR:
    return "*";
  case TokenType::SLASH:
    return "/";
  case TokenType::MOD:
    return "%";
  case TokenType::INC:
    return "++";
  case TokenType::DEC:
    return "--";
  case TokenType::COMMA:
    return ",";
  case TokenType::SEMICOLON:
    return ";";
  case TokenType::NOT:
    return "!";
  case TokenType::OPEN_BOX_BRACKET:
    return "[";
  case TokenType::CLOSE_BOX_BRACKET:
    return "]";
  case TokenType::OPEN_CURLY_BRACKET:
    return "{";
  case TokenType::CLOSE_CURLY_BRACKET:
    return "}";
  case TokenType::OPEN_PAREN:
    return "(";
  case TokenType::CLOSE_PAREN:
    return ")";
  default:
    return "<UNKNOWN>";
  }
}

struct Token {
  /// Data if any (digits, symbols, literals).
  std::string Data;

  /// Token type.
  TokenType Type;

  /// Position in source text.
  unsigned LineNo;

  /// Position in source text.
  unsigned ColumnNo;

  Token(std::string_view TheData, TokenType TheType, unsigned TheLineNo,
        unsigned TheColumnNo)
      : Data(TheData), Type(TheType), LineNo(TheLineNo), ColumnNo(TheColumnNo) {
  }

  bool operator==(const Token &rhs) const {
    return (Data == rhs.Data) && (Type == rhs.Type);
  }

  bool operator!=(const Token &rhs) const {
    return (Data != rhs.Data) || (Type != rhs.Type);
  }
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_LEX_TOKEN_HPP
