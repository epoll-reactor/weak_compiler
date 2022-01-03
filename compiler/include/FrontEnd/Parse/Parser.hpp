#ifndef WEAK_COMPILER_FRONTEND_PARSE_PARSER_HPP
#define WEAK_COMPILER_FRONTEND_PARSE_PARSER_HPP

#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include "FrontEnd/Lex/Token.hpp"
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

  std::unique_ptr<ASTNode> ParseVarDecl();

  /// Int, char, string, bool.
  const Token &ParseType();

  /// All from ParseType() or void.
  const Token &ParseReturnType();

  /// <Data type> <id>.
  std::unique_ptr<ASTNode> ParseParameter();

  /// (<Data type> <id> ,?)*
  std::vector<std::unique_ptr<ASTNode>> ParseParameterList();

  /// Block of code between '{' and '}'.
  std::unique_ptr<ASTCompoundStmt> ParseBlock();

  /// Block of code with break and continue statements.
  std::unique_ptr<ASTCompoundStmt> ParseIterationStmtBlock();

  /// Selection, iterative, jump, assignment statement
  /// or unary/binary operator.
  std::unique_ptr<ASTNode> ParseStatement();

  /// If statement.
  std::unique_ptr<ASTNode> ParseSelectionStatement();

  /// For, while or do-while statement.
  std::unique_ptr<ASTNode> ParseIterationStatement();

  std::unique_ptr<ASTNode> ParseForStatement();

  std::unique_ptr<ASTNode> ParseWhileStatement();

  std::unique_ptr<ASTNode> ParseDoWhileStatement();

  /// ParseStatement, break and continue statements.
  std::unique_ptr<ASTNode> ParseLoopStatement();

  /// Return statement.
  std::unique_ptr<ASTNode> ParseJumpStatement();

  /// Unary/binary statement or literal.
  std::unique_ptr<ASTNode> ParseExpression();

  std::unique_ptr<ASTNode> ParseAssignment();

  std::unique_ptr<ASTNode> ParseLogicalOr();

  std::unique_ptr<ASTNode> ParseLogicalAnd();

  std::unique_ptr<ASTNode> ParseInclusiveOr();

  std::unique_ptr<ASTNode> ParseExclusiveOr();

  std::unique_ptr<ASTNode> ParseAnd();

  std::unique_ptr<ASTNode> ParseEquality();

  std::unique_ptr<ASTNode> ParseRelational();

  std::unique_ptr<ASTNode> ParseShift();

  std::unique_ptr<ASTNode> ParseAdditive();

  std::unique_ptr<ASTNode> ParseMultiplicative();

  std::unique_ptr<ASTNode> ParseUnary();

  std::unique_ptr<ASTNode> ParsePostfix();

  std::unique_ptr<ASTNode> ParsePrimary();

  /// Integral, floating-point, string or boolean literal.
  std::unique_ptr<ASTNode> ParseConstant();

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

  void CheckIfHaveMoreTokens() const;

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
