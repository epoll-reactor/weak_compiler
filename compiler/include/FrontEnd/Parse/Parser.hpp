#ifndef WEAK_COMPILER_FRONTEND_PARSE_PARSER_HPP
#define WEAK_COMPILER_FRONTEND_PARSE_PARSER_HPP

#include "FrontEnd/Lex/Token.hpp"
#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include <vector>

namespace weak {
namespace frontEnd {

/// LL(1) Syntax analyzer.
class Parser {
public:
  Parser(const Token *TheBufferStart, const Token *TheBufferEnd);

  /// Transform token stream to AST.
  std::unique_ptr<ASTCompoundStmt> Parse();

private:
  std::unique_ptr<ASTNode> ParseFunctionDecl();

  /// Integral, floating-point, string or boolean literal.
  const Token &ParseConstant();

  /// Int, char, string, bool.
  const Token &ParseType();

  /// All from ParseType() or void.
  const Token &ParseReturnType();

  /// <Data type> <id>.
  std::unique_ptr<ASTNode> ParseParameter();

  /// Block of code between '{' and '}'.
  std::unique_ptr<ASTCompoundStmt> ParseBlock();

  /// Selection, iterative, jump, assignment statement
  /// or unary/binary operator.
  std::unique_ptr<ASTNode> ParseStatement();

  /// (<Data type> <id> ,?)*
  std::vector<std::unique_ptr<ASTNode>> ParseParameterList();

  /// Get current token from input range and move forward.
  const Token &PeekNext();

  /// Get current token from input range without moving to the next one.
  const Token &PeekCurrent() const;

  /// Return true and move current buffer pointer forward if current token
  /// matches any of expected, otherwise return false.
  bool Match(const std::vector<TokenType> &Expected);

  /// Return true and move current buffer pointer forward if current token
  /// matches given token type, otherwise return false.
  bool Match(TokenType Expected);

  /// Does the Match job, but terminates program with log message on error.
  const Token &Require(const std::vector<TokenType> &Expected);

  /// Does the Match job, but terminates program with log message on error.
  const Token &Require(TokenType Expected);

  /// First token in input stream.
  const Token *BufferStart;

  /// Last token in input stream.
  const Token *BufferEnd;

  /// Current token to be processed.
  const Token *CurrentBufferPtr;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_PARSE_PARSER_HPP
