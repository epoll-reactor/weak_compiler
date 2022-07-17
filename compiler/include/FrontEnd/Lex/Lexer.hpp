/* Lexer.hpp - Implementation of lexical analyzer.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_FRONTEND_LEX_LEXER_HPP
#define WEAK_COMPILER_FRONTEND_LEX_LEXER_HPP

#include "FrontEnd/Lex/Token.hpp"
#include <vector>

namespace weak {
namespace frontEnd {

/// \brief Lexical analyzer.
///
/// Provides interface to transform plain text
/// into a stream of tokens.
class Lexer {
public:
  Lexer(const char *TheBufferStart, const char *TheBufferEnd);

  /// Walk through input text and generate stream of tokens.
  const std::vector<Token> &Analyze();

private:
  Token AnalyzeDigit();
  Token AnalyzeStringLiteral();
  Token AnalyzeSymbol();
  Token AnalyzeOperator();

  /// Ignore C-style one-line and multi-line comments.
  void ProcessComment();

  void ProcessOneLineComment();
  void ProcessMultiLineComment();

  /// Get current character from input range and move forward.
  char PeekNext();

  /// Get current character from input without moving to the next one.
  char PeekCurrent() const;

  Token MakeToken(std::string_view Data, TokenType Type) const;

  /// Internal buffer.
  std::vector<Token> ProcessedTokens;

  /// First symbol in buffer.
  const char *BufferStart;

  /// Last symbol in buffer (null-terminator).
  const char *BufferEnd;

  /// Current symbol to be lexed.
  const char *CurrentBufferPtr;

  /// Line number (used for error generating).
  unsigned CurrentLineNo;

  /// Column number (used for error generating).
  unsigned CurrentColumnNo;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_LEX_LEXER_HPP
