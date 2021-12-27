#ifndef WEAK_COMPILER_FRONTEND_PARSE_PARSER_HPP
#define WEAK_COMPILER_FRONTEND_PARSE_PARSER_HPP

#include "FrontEnd/Lex/Token.hpp"
#include <vector>

namespace weak {
namespace frontEnd {

class Parser {
public:
  Parser(const Token* TheBufferStart, const Token* TheBufferEnd);

  // AST should be implemented before the parser.

private:
  /// Get current token from input range and move forward.
  const Token& PeekNext();

  /// Get current token from input range without moving to the next one.
  const Token& PeekCurrent() const;

  /// First token in input stream.
  const Token* BufferStart;

  /// Last token in input stream.
  const Token* BufferEnd;

  /// Current token to be processed.
  const Token* CurrentBufferPtr;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_PARSE_PARSER_HPP
