/* Parser.cpp - Implementation of syntax analyzer.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/Parse/Parser.hpp"
#include "FrontEnd/AST/ASTBinaryOperator.hpp"
#include "FrontEnd/AST/ASTBooleanLiteral.hpp"
#include "FrontEnd/AST/ASTBreakStmt.hpp"
#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include "FrontEnd/AST/ASTContinueStmt.hpp"
#include "FrontEnd/AST/ASTDoWhileStmt.hpp"
#include "FrontEnd/AST/ASTFloatingPointLiteral.hpp"
#include "FrontEnd/AST/ASTForStmt.hpp"
#include "FrontEnd/AST/ASTFunctionCall.hpp"
#include "FrontEnd/AST/ASTFunctionDecl.hpp"
#include "FrontEnd/AST/ASTIfStmt.hpp"
#include "FrontEnd/AST/ASTIntegerLiteral.hpp"
#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/AST/ASTReturnStmt.hpp"
#include "FrontEnd/AST/ASTStringLiteral.hpp"
#include "FrontEnd/AST/ASTSymbol.hpp"
#include "FrontEnd/AST/ASTUnaryOperator.hpp"
#include "FrontEnd/AST/ASTVarDecl.hpp"
#include "FrontEnd/AST/ASTWhileStmt.hpp"
#include "Utility/Diagnostic.hpp"
#include <cassert>

static std::string
TokensToString(const std::vector<weak::frontEnd::TokenType> &Tokens) {
  std::string result;

  for (const auto &T : Tokens) {
    result += "(";
    result += TokenToString(T);
    result += ")";
    result += ", ";
  }

  if (!result.empty()) {
    result.pop_back();
    result.pop_back();
  }

  return result;
}

namespace weak {
namespace frontEnd {

Parser::Parser(const Token *TheBufferStart, const Token *TheBufferEnd)
    : BufferStart(TheBufferStart), BufferEnd(TheBufferEnd),
      CurrentBufferPtr(BufferStart) {
  assert(BufferStart);
  assert(BufferEnd);
  assert(BufferStart <= BufferEnd);
}

std::unique_ptr<ASTCompoundStmt> Parser::Parse() {
  std::vector<std::unique_ptr<ASTNode>> GlobalEntities;
  while (CurrentBufferPtr != BufferEnd) {
    const Token &Current = PeekCurrent();
    switch (Current.Type) {
    case TokenType::VOID:
    case TokenType::INT:
    case TokenType::CHAR:
    case TokenType::STRING:
    case TokenType::BOOLEAN: // Fall through.
      GlobalEntities.push_back(ParseFunctionDecl());
      break;
    default:
      CompileError(Current.LineNo, Current.ColumnNo)
          << "Functions as global statements supported only.";
      break;
    }
  }
  return std::make_unique<ASTCompoundStmt>(std::move(GlobalEntities));
}

std::unique_ptr<ASTNode> Parser::ParseFunctionDecl() {
  /// Guaranteed data type, no checks needed.
  const Token &ReturnType = ParseReturnType();
  const Token &FunctionName = PeekNext();
  std::vector<std::unique_ptr<ASTNode>> ParameterList;

  if (FunctionName.Type != TokenType::SYMBOL)
    CompileError(FunctionName.LineNo, FunctionName.ColumnNo)
        << "Function name expected.";

  Require(TokenType::OPEN_PAREN);
  ParameterList = ParseParameterList();
  Require(TokenType::CLOSE_PAREN);

  auto Block = ParseBlock();

  return std::make_unique<ASTFunctionDecl>(
      ReturnType.Type, std::string(FunctionName.Data), std::move(ParameterList),
      std::move(Block), ReturnType.LineNo, ReturnType.ColumnNo);
}

std::unique_ptr<ASTNode> Parser::ParseFunctionCall() {
  const Token &FunctionName = PeekNext();
  std::string Name = FunctionName.Data;
  std::vector<std::unique_ptr<ASTNode>> Arguments;

  Require(TokenType::OPEN_PAREN);

  if (PeekNext().Type == TokenType::CLOSE_PAREN) {
    return std::make_unique<ASTFunctionCall>(
        std::move(Name), std::move(Arguments), FunctionName.LineNo,
        FunctionName.ColumnNo);
  }

  --CurrentBufferPtr;
  while (PeekCurrent().Type != TokenType::CLOSE_PAREN) {
    Arguments.push_back(ParseLogicalOr());
    Require({TokenType::CLOSE_PAREN, TokenType::COMMA});
    if ((CurrentBufferPtr - 1)->Type == TokenType::CLOSE_PAREN) {
      /// Move back to token before '('.
      --CurrentBufferPtr;
      break;
    }
  }

  Require(TokenType::CLOSE_PAREN);

  return std::make_unique<ASTFunctionCall>(
      std::move(Name), std::move(Arguments), FunctionName.LineNo,
      FunctionName.ColumnNo);
}

std::unique_ptr<ASTNode> Parser::ParseVarDecl() {
  const Token &DataType = ParseType();
  std::string VariableName = PeekNext().Data;
  const Token &Current = PeekNext(); // Assignment op.

  if (Current.Type == TokenType::ASSIGN) {
    return std::make_unique<ASTVarDecl>(DataType.Type, std::move(VariableName),
                                        ParseLogicalOr(), DataType.LineNo,
                                        DataType.ColumnNo);
  }

  CompileError(Current.LineNo, Current.ColumnNo)
      << "Assignment operator expected.";
  UnreachablePoint();
}

const Token &Parser::ParseType() {
  switch (const Token &Current = PeekCurrent(); Current.Type) {
  case TokenType::INT:
  case TokenType::CHAR:
  case TokenType::FLOAT:
  case TokenType::STRING:
  case TokenType::BOOLEAN: // Fall through.
    PeekNext();
    return Current;
  default:
    CompileError(Current.LineNo, Current.ColumnNo) << "Data type expected.";
    UnreachablePoint();
  }
}

const Token &Parser::ParseReturnType() {
  const Token &Current = PeekCurrent();
  if (Current.Type != TokenType::VOID)
    return ParseType();
  PeekNext();
  return Current;
}

std::unique_ptr<ASTNode> Parser::ParseParameter() {
  const Token &DataType = ParseType();
  const Token &VariableName = PeekNext();

  if (VariableName.Type != TokenType::SYMBOL)
    CompileError(VariableName.LineNo, VariableName.ColumnNo)
        << "Variable name expected.";

  return std::make_unique<ASTVarDecl>(
      DataType.Type, std::string(VariableName.Data),
      /*DeclareBody=*/nullptr, DataType.LineNo, DataType.ColumnNo);
}

std::vector<std::unique_ptr<ASTNode>> Parser::ParseParameterList() {
  std::vector<std::unique_ptr<ASTNode>> ParameterList;
  if (PeekNext().Type == TokenType::CLOSE_PAREN) {
    --CurrentBufferPtr;
    return ParameterList;
  }

  --CurrentBufferPtr;
  while (PeekCurrent().Type != TokenType::CLOSE_PAREN) {
    ParameterList.push_back(ParseParameter());
    Match({TokenType::CLOSE_PAREN, TokenType::COMMA});
    if ((CurrentBufferPtr - 1)->Type == TokenType::CLOSE_PAREN) {
      /// Move back to token before '('.
      --CurrentBufferPtr;
      break;
    }
  }
  return ParameterList;
}

std::unique_ptr<ASTCompoundStmt> Parser::ParseBlock() {
  std::vector<std::unique_ptr<ASTNode>> Statements;

  const Token &BeginOfBlock = Require(TokenType::OPEN_CURLY_BRACKET);
  while (PeekCurrent().Type != TokenType::CLOSE_CURLY_BRACKET) {
    Statements.push_back(ParseStatement());
    switch (const ASTType Type = Statements.back()->GetASTType(); Type) {
    case ASTType::BINARY:
    case ASTType::POSTFIX_UNARY:
    case ASTType::PREFIX_UNARY:
    case ASTType::SYMBOL:
    case ASTType::RETURN_STMT:
    case ASTType::DO_WHILE_STMT:
    case ASTType::VAR_DECL:
    case ASTType::FUNCTION_CALL: // Fall through.
      Require(TokenType::SEMICOLON);
      break;
    default:
      break;
    }
  }
  Require(TokenType::CLOSE_CURLY_BRACKET);

  return std::make_unique<ASTCompoundStmt>(
      std::move(Statements), BeginOfBlock.LineNo, BeginOfBlock.ColumnNo);
}

std::unique_ptr<ASTCompoundStmt> Parser::ParseIterationStmtBlock() {
  std::vector<std::unique_ptr<ASTNode>> Statements;

  const Token &BeginOfBlock = Require(TokenType::OPEN_CURLY_BRACKET);
  while (PeekCurrent().Type != TokenType::CLOSE_CURLY_BRACKET) {
    Statements.push_back(ParseLoopStatement());
    switch (const ASTType Type = Statements.back()->GetASTType(); Type) {
    case ASTType::BINARY:
    case ASTType::POSTFIX_UNARY:
    case ASTType::PREFIX_UNARY:
    case ASTType::SYMBOL:
    case ASTType::RETURN_STMT:
    case ASTType::BREAK_STMT:
    case ASTType::CONTINUE_STMT:
    case ASTType::DO_WHILE_STMT:
    case ASTType::VAR_DECL:
    case ASTType::FUNCTION_CALL: // Fall through.
      Require(TokenType::SEMICOLON);
      break;
    default:
      break;
    }
  }
  Require(TokenType::CLOSE_CURLY_BRACKET);

  return std::make_unique<ASTCompoundStmt>(
      std::move(Statements), BeginOfBlock.LineNo, BeginOfBlock.ColumnNo);
}

std::unique_ptr<ASTNode> Parser::ParseStatement() {
  switch (const Token &Current = PeekCurrent(); Current.Type) {
  case TokenType::IF:
    return ParseSelectionStatement();
  case TokenType::FOR:
  case TokenType::DO:
  case TokenType::WHILE: // Fall through.
    return ParseIterationStatement();
  case TokenType::RETURN:
    return ParseJumpStatement();
  case TokenType::INT:
  case TokenType::CHAR:
  case TokenType::FLOAT:
  case TokenType::STRING:
  case TokenType::BOOLEAN: // Fall through.
    return ParseVarDecl();
  case TokenType::SYMBOL:
    return ParseExpression();
  case TokenType::INC:
  case TokenType::DEC: // Fall through.
    return ParsePrefixUnary();
  default:
    CompileError(Current.LineNo, Current.ColumnNo)
        << "Unexpected token: " << TokenToString(Current.Type);
    UnreachablePoint();
  }
}

std::unique_ptr<ASTNode> Parser::ParseSelectionStatement() {
  std::unique_ptr<ASTNode> Condition;
  std::unique_ptr<ASTCompoundStmt> ThenBody;
  std::unique_ptr<ASTCompoundStmt> ElseBody;

  const Token &BeginOfSelectionStmt = Require(TokenType::IF);
  Require(TokenType::OPEN_PAREN);
  Condition = ParseLogicalOr();
  Require(TokenType::CLOSE_PAREN);
  ThenBody = ParseBlock();
  if (Match(TokenType::ELSE)) {
    ElseBody = ParseBlock();
  }

  return std::make_unique<ASTIfStmt>(
      std::move(Condition), std::move(ThenBody), std::move(ElseBody),
      BeginOfSelectionStmt.LineNo, BeginOfSelectionStmt.ColumnNo);
}

std::unique_ptr<ASTNode> Parser::ParseIterationStatement() {
  switch (const Token &Current = PeekCurrent(); Current.Type) {
  case TokenType::FOR:
    return ParseForStatement();
  case TokenType::DO:
    return ParseDoWhileStatement();
  case TokenType::WHILE:
    return ParseWhileStatement();
  default:
    CompileError(Current.LineNo, Current.ColumnNo) << "Should not reach here.";
    UnreachablePoint();
  }
}

std::unique_ptr<ASTNode> Parser::ParseForStatement() {
  const Token &ForStmtBegin = Require(TokenType::FOR);
  Require(TokenType::OPEN_PAREN);

  std::unique_ptr<ASTNode> Init;
  if (PeekNext().Type != TokenType::SEMICOLON) {
    --CurrentBufferPtr;
    Init = ParseExpression();
    PeekNext();
  }

  std::unique_ptr<ASTNode> Condition;
  if (PeekNext().Type != TokenType::SEMICOLON) {
    --CurrentBufferPtr;
    Condition = ParseLogicalOr();
    PeekNext();
  }

  std::unique_ptr<ASTNode> Increment;
  if (PeekNext().Type != TokenType::CLOSE_PAREN) {
    --CurrentBufferPtr;
    Increment = ParseLogicalOr();
    PeekNext();
  }
  --CurrentBufferPtr;

  Require(TokenType::CLOSE_PAREN);
  auto Body = ParseIterationStmtBlock();

  return std::make_unique<ASTForStmt>(
      std::move(Init), std::move(Condition), std::move(Increment),
      std::move(Body), ForStmtBegin.LineNo, ForStmtBegin.ColumnNo);
}

std::unique_ptr<ASTNode> Parser::ParseDoWhileStatement() {
  const Token &DoWhileBegin = Require(TokenType::DO);
  auto Body = ParseIterationStmtBlock();
  Require(TokenType::WHILE);

  Require(TokenType::OPEN_PAREN);
  auto Condition = ParseLogicalOr();
  Require(TokenType::CLOSE_PAREN);

  return std::make_unique<ASTDoWhileStmt>(std::move(Body), std::move(Condition),
                                          DoWhileBegin.LineNo,
                                          DoWhileBegin.ColumnNo);
}

std::unique_ptr<ASTNode> Parser::ParseWhileStatement() {
  const Token &WhileBegin = Require(TokenType::WHILE);
  Require(TokenType::OPEN_PAREN);
  auto Condition = ParseLogicalOr();
  Require(TokenType::CLOSE_PAREN);

  auto Body = ParseIterationStmtBlock();

  return std::make_unique<ASTWhileStmt>(std::move(Condition), std::move(Body),
                                        WhileBegin.LineNo, WhileBegin.ColumnNo);
}

std::unique_ptr<ASTNode> Parser::ParseLoopStatement() {
  switch (const Token &Current = PeekNext(); Current.Type) {
  case TokenType::BREAK:
    return std::make_unique<ASTBreakStmt>(Current.LineNo, Current.ColumnNo);
  case TokenType::CONTINUE:
    return std::make_unique<ASTContinueStmt>(Current.LineNo, Current.ColumnNo);
  default:
    --CurrentBufferPtr;
    return ParseStatement();
  }
}

std::unique_ptr<ASTNode> Parser::ParseJumpStatement() {
  const Token &ReturnStmt = Require(TokenType::RETURN);
  if (Match(TokenType::SEMICOLON)) {
    // Rollback to allow match ';' in block parse function.
    --CurrentBufferPtr;
    return std::make_unique<ASTReturnStmt>(nullptr, ReturnStmt.LineNo,
                                           ReturnStmt.ColumnNo);
  }
  // We want to forbid expressions like int var = var = var, so we
  // expect the first expression to have the precedence is lower than
  // the assignment operator.
  return std::make_unique<ASTReturnStmt>(ParseLogicalOr(), ReturnStmt.LineNo,
                                         ReturnStmt.ColumnNo);
}

std::unique_ptr<ASTNode> Parser::ParseExpression() {
  switch (PeekCurrent().Type) {
  case TokenType::INT:
  case TokenType::CHAR:
  case TokenType::STRING:
  case TokenType::BOOLEAN: // Fall through.
    return ParseVarDecl();
  default:
    break;
  }
  PeekNext();
  switch (PeekCurrent().Type) {
  case TokenType::OPEN_PAREN:
    --CurrentBufferPtr;
    return ParseFunctionCall();
  default:
    --CurrentBufferPtr;
    return ParseAssignment();
  }
}

std::unique_ptr<ASTNode> Parser::ParseAssignment() {
  auto Expr = ParseLogicalOr();
  while (true) {
    switch (const Token &Current = PeekNext(); Current.Type) {
    case TokenType::ASSIGN:
    case TokenType::MUL_ASSIGN:
    case TokenType::DIV_ASSIGN:
    case TokenType::MOD_ASSIGN:
    case TokenType::PLUS_ASSIGN:
    case TokenType::MINUS_ASSIGN:
    case TokenType::SHL_ASSIGN:
    case TokenType::SHR_ASSIGN:
    case TokenType::BIT_AND_ASSIGN:
    case TokenType::BIT_OR_ASSIGN:
    case TokenType::XOR_ASSIGN:
      Expr = std::make_unique<ASTBinaryOperator>(
          Current.Type, std::move(Expr), ParseAssignment(), Current.LineNo,
          Current.ColumnNo);
      continue;
    default:
      --CurrentBufferPtr;
      break;
    }
    break;
  }
  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseLogicalOr() {
  auto Expr = ParseLogicalAnd();
  while (true) {
    switch (const Token &Current = PeekNext(); Current.Type) {
    case TokenType::OR:
      Expr = std::make_unique<ASTBinaryOperator>(
          Current.Type, std::move(Expr), ParseLogicalOr(), Current.LineNo,
          Current.ColumnNo);
      continue;
    default:
      --CurrentBufferPtr;
      break;
    }
    break;
  }
  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseLogicalAnd() {
  auto Expr = ParseInclusiveOr();
  while (true) {
    switch (const Token &Current = PeekNext(); Current.Type) {
    case TokenType::AND:
      Expr = std::make_unique<ASTBinaryOperator>(
          Current.Type, std::move(Expr), ParseLogicalAnd(), Current.LineNo,
          Current.ColumnNo);
      continue;
    default:
      --CurrentBufferPtr;
      break;
    }
    break;
  }
  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseInclusiveOr() {
  auto Expr = ParseExclusiveOr();
  while (true) {
    switch (const Token &Current = PeekNext(); Current.Type) {
    case TokenType::BIT_OR:
      Expr = std::make_unique<ASTBinaryOperator>(
          Current.Type, std::move(Expr), ParseInclusiveOr(), Current.LineNo,
          Current.ColumnNo);
      continue;
    default:
      --CurrentBufferPtr;
      break;
    }
    break;
  }
  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseExclusiveOr() {
  auto Expr = ParseAnd();
  while (true) {
    switch (const Token &Current = PeekNext(); Current.Type) {
    case TokenType::XOR:
      Expr = std::make_unique<ASTBinaryOperator>(
          Current.Type, std::move(Expr), ParseExclusiveOr(), Current.LineNo,
          Current.ColumnNo);
      continue;
    default:
      --CurrentBufferPtr;
      break;
    }
    break;
  }
  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseAnd() {
  auto Expr = ParseEquality();
  while (true) {
    switch (const Token &Current = PeekNext(); Current.Type) {
    case TokenType::BIT_AND:
      Expr = std::make_unique<ASTBinaryOperator>(Current.Type, std::move(Expr),
                                                 ParseAnd(), Current.LineNo,
                                                 Current.ColumnNo);
      continue;
    default:
      --CurrentBufferPtr;
      break;
    }
    break;
  }
  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseEquality() {
  auto Expr = ParseRelational();
  while (true) {
    switch (const Token &Current = PeekNext(); Current.Type) {
    case TokenType::EQ:
    case TokenType::NEQ:
      Expr = std::make_unique<ASTBinaryOperator>(
          Current.Type, std::move(Expr), ParseEquality(), Current.LineNo,
          Current.ColumnNo);
      continue;
    default:
      --CurrentBufferPtr;
      break;
    }
    break;
  }
  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseRelational() {
  auto Expr = ParseShift();
  while (true) {
    switch (const Token &Current = PeekNext(); Current.Type) {
    case TokenType::GT:
    case TokenType::LT:
    case TokenType::GE:
    case TokenType::LE:
      Expr = std::make_unique<ASTBinaryOperator>(
          Current.Type, std::move(Expr), ParseRelational(), Current.LineNo,
          Current.ColumnNo);
      continue;
    default:
      --CurrentBufferPtr;
      break;
    }
    break;
  }
  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseShift() {
  auto Expr = ParseAdditive();
  while (true) {
    switch (const Token &Current = PeekNext(); Current.Type) {
    case TokenType::SHL:
    case TokenType::SHR:
      Expr = std::make_unique<ASTBinaryOperator>(Current.Type, std::move(Expr),
                                                 ParseShift(), Current.LineNo,
                                                 Current.ColumnNo);
      continue;
    default:
      --CurrentBufferPtr;
      break;
    }
    break;
  }
  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseAdditive() {
  auto Expr = ParseMultiplicative();
  while (true) {
    switch (const Token &Current = PeekNext(); Current.Type) {
    case TokenType::PLUS:
    case TokenType::MINUS:
      Expr = std::make_unique<ASTBinaryOperator>(
          Current.Type, std::move(Expr), ParseAdditive(), Current.LineNo,
          Current.ColumnNo);
      continue;
    default:
      --CurrentBufferPtr;
      break;
    }
    break;
  }
  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseMultiplicative() {
  auto Expr = ParsePrefixUnary();
  while (true) {
    switch (const Token &Current = PeekNext(); Current.Type) {
    case TokenType::STAR:
    case TokenType::SLASH:
    case TokenType::MOD:
      Expr = std::make_unique<ASTBinaryOperator>(
          Current.Type, std::move(Expr), ParseMultiplicative(), Current.LineNo,
          Current.ColumnNo);
      continue;
    default:
      --CurrentBufferPtr;
      break;
    }
    break;
  }
  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParsePrefixUnary() {
  switch (const Token Current = PeekNext(); Current.Type) {
  case TokenType::INC:
  case TokenType::DEC:
    return std::make_unique<ASTUnaryOperator>(
        ASTUnaryOperator::UnaryType::PREFIX, Current.Type, ParsePostfixUnary(),
        Current.LineNo, Current.ColumnNo);
  default:
    // Rollback current token pointer because there's no unary operator.
    --CurrentBufferPtr;
    return ParsePostfixUnary();
  }
}

std::unique_ptr<ASTNode> Parser::ParsePostfixUnary() {
  auto Expr = ParsePrimary();
  while (true) {
    switch (const Token &Current = PeekNext(); Current.Type) {
    case TokenType::INC:
    case TokenType::DEC:
      Expr = std::make_unique<ASTUnaryOperator>(
          ASTUnaryOperator::UnaryType::POSTFIX, Current.Type, std::move(Expr),
          Current.LineNo, Current.ColumnNo);
      continue;
    default:
      --CurrentBufferPtr;
      break;
    }
    break;
  }
  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParsePrimary() {
  switch (const Token &Current = PeekNext(); Current.Type) {
  case TokenType::SYMBOL:
    return std::make_unique<ASTSymbol>(Current.Data, Current.LineNo,
                                       Current.ColumnNo);
  case TokenType::OPEN_PAREN: {
    /// We expect all binary/unary/constant statements expect assignment.
    auto Expr = ParseLogicalOr();
    Require(TokenType::CLOSE_PAREN);
    return Expr;
  }
  default:
    --CurrentBufferPtr;
    return ParseConstant();
  }
}

std::unique_ptr<ASTNode> Parser::ParseConstant() {
  switch (const Token &Current = PeekNext(); Current.Type) {
  case TokenType::INTEGRAL_LITERAL:
    return std::make_unique<ASTIntegerLiteral>(
        std::stoi(Current.Data), Current.LineNo, Current.ColumnNo);

  case TokenType::FLOATING_POINT_LITERAL:
    return std::make_unique<ASTFloatingPointLiteral>(
        std::stod(Current.Data), Current.LineNo, Current.ColumnNo);

  case TokenType::STRING_LITERAL:
    return std::make_unique<ASTStringLiteral>(Current.Data, Current.LineNo,
                                              Current.ColumnNo);

  case TokenType::FALSE:
  case TokenType::TRUE:
    return std::make_unique<ASTBooleanLiteral>(
        Current.Type == TokenType::TRUE, Current.LineNo, Current.ColumnNo);

  default:
    CompileError(Current.LineNo, Current.ColumnNo) << "Literal expected.";
    UnreachablePoint();
  }
}

const Token &Parser::PeekNext() {
  CheckIfHaveMoreTokens();
  return *CurrentBufferPtr++;
}

const Token &Parser::PeekCurrent() const {
  CheckIfHaveMoreTokens();
  return *CurrentBufferPtr;
}

bool Parser::Match(const std::vector<TokenType> &Expected) {
  CheckIfHaveMoreTokens();
  for (TokenType Token : Expected) {
    if (CurrentBufferPtr == BufferEnd)
      return false;
    if (PeekCurrent().Type == Token) {
      PeekNext();
      return true;
    }
  }
  return false;
}

bool Parser::Match(TokenType Expected) {
  return Match(std::vector<TokenType>{Expected});
}

const Token &Parser::Require(const std::vector<TokenType> &Expected) {
  CheckIfHaveMoreTokens();
  if (Match(Expected)) {
    /// Something from vector successfully matched and located in previous
    /// token.
    return *(CurrentBufferPtr - 1);
  }

  CompileError(CurrentBufferPtr->LineNo, CurrentBufferPtr->ColumnNo)
      << "Expected " << TokensToString(Expected) << ", got "
      << TokenToString(CurrentBufferPtr->Type);
  UnreachablePoint();
}

const Token &Parser::Require(TokenType Expected) {
  return Require(std::vector<TokenType>{Expected});
}

void Parser::CheckIfHaveMoreTokens() const {
  if (CurrentBufferPtr == BufferEnd) {
    CompileError(CurrentBufferPtr->LineNo, CurrentBufferPtr->LineNo)
        << "End of buffer reached.";
    UnreachablePoint();
  }
}

} // namespace frontEnd
} // namespace weak