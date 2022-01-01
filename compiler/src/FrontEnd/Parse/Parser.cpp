#include "FrontEnd/Parse/Parser.hpp"
#include "FrontEnd/AST/ASTBinaryOperator.hpp"
#include "FrontEnd/AST/ASTBooleanLiteral.hpp"
#include "FrontEnd/AST/ASTBreakStmt.hpp"
#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include "FrontEnd/AST/ASTContinueStmt.hpp"
#include "FrontEnd/AST/ASTDoWhileStmt.hpp"
#include "FrontEnd/AST/ASTFloatingPointLiteral.hpp"
#include "FrontEnd/AST/ASTForStmt.hpp"
#include "FrontEnd/AST/ASTFunctionDecl.hpp"
#include "FrontEnd/AST/ASTIfStmt.hpp"
#include "FrontEnd/AST/ASTIntegerLiteral.hpp"
#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/AST/ASTReturnStmt.hpp"
#include "FrontEnd/AST/ASTStringLiteral.hpp"
#include "FrontEnd/AST/ASTSymbol.hpp"
#include "FrontEnd/AST/ASTUnaryOperator.hpp"
#include "FrontEnd/AST/ASTVarDecl.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"
#include "FrontEnd/AST/ASTWhileStmt.hpp"
#include "Utility/Diagnostic.hpp"
#include <cassert>
#include <iostream>

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
      DiagnosticError(Current.LineNo, Current.ColumnNo)
          << "Functions as global statements supported only.";
      break;
    }
  }
  return std::make_unique<ASTCompoundStmt>(std::move(GlobalEntities));
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

  case TokenType::BOOLEAN: // Fall through.
    return std::make_unique<ASTBooleanLiteral>(
        std::stoi(Current.Data), Current.LineNo, Current.ColumnNo);

  default:
    DiagnosticError(Current.LineNo, Current.ColumnNo) << "Literal expected.";
    UnreachablePoint();
  }
}

std::unique_ptr<ASTNode> Parser::ParseFunctionDecl() {
  /// Guaranteed data type, no checks needed.
  const Token &ReturnType = ParseReturnType();
  const Token &FunctionName = PeekNext();
  std::vector<std::unique_ptr<ASTNode>> ParameterList;

  if (FunctionName.Type != TokenType::SYMBOL)
    DiagnosticError(FunctionName.LineNo, FunctionName.ColumnNo)
        << "Function name expected.";

  Require(TokenType::OPEN_PAREN);
  ParameterList = ParseParameterList();
  Require(TokenType::CLOSE_PAREN);

  auto Block = ParseBlock();

  return std::make_unique<ASTFunctionDecl>(
      ReturnType.Type, std::string(FunctionName.Data), std::move(ParameterList),
      std::move(Block));
}

const Token &Parser::ParseType() {
  const Token &Current = PeekCurrent();
  switch (Current.Type) {
  case TokenType::INT:
  case TokenType::CHAR:
  case TokenType::STRING:
  case TokenType::BOOLEAN: // Fall through.
    PeekNext();
    return Current;
  default:
    DiagnosticError(Current.LineNo, Current.ColumnNo) << "Data type expected.";
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
    DiagnosticError(VariableName.LineNo, VariableName.ColumnNo)
        << "Variable name expected.";

  return std::make_unique<ASTVarDecl>(DataType.Type,
                                      std::string(VariableName.Data),
                                      /*DeclareBody=*/nullptr);
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

  Require(TokenType::OPEN_CURLY_BRACKET);

  while (PeekCurrent().Type != TokenType::CLOSE_CURLY_BRACKET) {
    Statements.push_back(ParseStatement());
    switch (const ASTType Type = Statements.back()->GetASTType(); Type) {
    case ASTType::BINARY:
    case ASTType::POSTFIX_UNARY:
    case ASTType::PREFIX_UNARY:
    case ASTType::SYMBOL: // Fall through.

      Require(TokenType::SEMICOLON);
      break;
    default:
      break;
    }
  }

  Require(TokenType::CLOSE_CURLY_BRACKET);

  return std::make_unique<ASTCompoundStmt>(std::move(Statements));
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
  default:
    return ParseExpression();
  }
}

std::unique_ptr<ASTNode> Parser::ParseSelectionStatement() {
  std::unique_ptr<ASTNode> Condition;
  std::unique_ptr<ASTCompoundStmt> ThenBody;
  std::unique_ptr<ASTCompoundStmt> ElseBody;

  Require(TokenType::IF);
  Require(TokenType::OPEN_PAREN);
  Condition = ParseExpression();
  Require(TokenType::CLOSE_PAREN);
  ThenBody = ParseBlock();
  if (Match(TokenType::ELSE)) {
    ElseBody = ParseBlock();
  }

  return std::make_unique<ASTIfStmt>(std::move(Condition), std::move(ThenBody),
                                     std::move(ElseBody));
}

std::unique_ptr<ASTNode> Parser::ParseIterationStatement() {
  switch (const Token &Current = PeekCurrent(); Current.Type) {
  case TokenType::FOR: {
    Require(TokenType::FOR);
    Require(TokenType::OPEN_PAREN);

    std::unique_ptr<ASTNode> Init;
    if (PeekNext().Type != TokenType::SEMICOLON) {
      --CurrentBufferPtr;
      Init = ParseMultiplicative();
      PeekNext();
    }

    std::unique_ptr<ASTNode> Condition;
    if (PeekNext().Type != TokenType::SEMICOLON) {
      --CurrentBufferPtr;
      Condition = ParseExpression();
      PeekNext();
    }

    std::unique_ptr<ASTNode> Increment;
    if (PeekNext().Type != TokenType::CLOSE_PAREN) {
      --CurrentBufferPtr;
      Increment = ParseExpression();
      PeekNext();
    }
    --CurrentBufferPtr;

    Require(TokenType::CLOSE_PAREN);
    auto Body = ParseBlock();

    return std::make_unique<ASTForStmt>(std::move(Init), std::move(Condition),
                                        std::move(Increment), std::move(Body));
  }
  case TokenType::DO: {
    Require(TokenType::DO);

    auto Body = ParseBlock();

    Require(TokenType::WHILE);

    Require(TokenType::OPEN_PAREN);
    auto Condition = ParseExpression();
    Require(TokenType::CLOSE_PAREN);

    return std::make_unique<ASTDoWhileStmt>(std::move(Body),
                                            std::move(Condition));
  }
  case TokenType::WHILE: {
    Require(TokenType::WHILE);
    Require(TokenType::OPEN_PAREN);
    auto Condition = ParseExpression();
    Require(TokenType::CLOSE_PAREN);

    auto Body = ParseBlock();

    return std::make_unique<ASTWhileStmt>(std::move(Condition),
                                          std::move(Body));
  }
  default:
    UnreachablePoint();
  }
}

std::unique_ptr<ASTNode> Parser::ParseJumpStatement() {
  Require(TokenType::RETURN);

  auto Body = ParseExpression();

  return std::make_unique<ASTReturnStmt>(std::move(Body));
}

// std::unique_ptr<ASTNode> Parser::ParseExpression() { return
// ParseAssignment(); }

// Just for testing, parse variable name only.
std::unique_ptr<ASTNode> Parser::ParseExpression() {
  const Token &Current = PeekNext();
  if (Current.Type == TokenType::SYMBOL) {
    return std::make_unique<ASTSymbol>(Current.Data);
  }

  DiagnosticWarning(Current.LineNo, Current.ColumnNo)
      << TokenToString(Current.Type);

  DiagnosticWarning(Current.LineNo, Current.ColumnNo)
      << "Current: " << TokenToString(CurrentBufferPtr->Type);

  DiagnosticError(Current.LineNo, Current.ColumnNo) << "Symbol name expected.";
  UnreachablePoint();
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
      Expr = std::make_unique<ASTBinaryOperator>(Current.Type, std::move(Expr),
                                                 ParseLogicalOr());
      continue;
    default:
      break;
    }
    break;
  }

  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseLogicalOr() {
  auto Expr = ParseLogicalAnd();

  while (true) {
    if (const Token &Current = PeekNext(); Current.Type == TokenType::OR) {
      Expr = std::make_unique<ASTBinaryOperator>(Current.Type, std::move(Expr),
                                                 ParseLogicalAnd());
      continue;
    }
    break;
  }

  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseLogicalAnd() {
  auto Expr = ParseInclusiveOr();

  while (true) {
    if (const Token &Current = PeekNext(); Current.Type == TokenType::AND) {
      Expr = std::make_unique<ASTBinaryOperator>(Current.Type, std::move(Expr),
                                                 ParseInclusiveOr());
      continue;
    }
    break;
  }

  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseInclusiveOr() {
  auto Expr = ParseExclusiveOr();

  while (true) {
    if (const Token &Current = PeekNext(); Current.Type == TokenType::OR) {
      Expr = std::make_unique<ASTBinaryOperator>(Current.Type, std::move(Expr),
                                                 ParseExclusiveOr());
      continue;
    }
    break;
  }

  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseExclusiveOr() {
  auto Expr = ParseAnd();

  while (true) {
    if (const Token &Current = PeekNext(); Current.Type == TokenType::XOR) {
      Expr = std::make_unique<ASTBinaryOperator>(Current.Type, std::move(Expr),
                                                 ParseAnd());
      continue;
    }
    break;
  }

  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseAnd() {
  auto Expr = ParseEquality();

  while (true) {
    if (const Token &Current = PeekNext(); Current.Type == TokenType::BIT_AND) {
      Expr = std::make_unique<ASTBinaryOperator>(Current.Type, std::move(Expr),
                                                 ParseEquality());
      continue;
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
      Expr = std::make_unique<ASTBinaryOperator>(Current.Type, std::move(Expr),
                                                 ParseRelational());
      continue;
    default:
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
      Expr = std::make_unique<ASTBinaryOperator>(Current.Type, std::move(Expr),
                                                 ParseShift());
      continue;
    default:
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
                                                 ParseAdditive());
      continue;
    default:
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
      Expr = std::make_unique<ASTBinaryOperator>(Current.Type, std::move(Expr),
                                                 ParseMultiplicative());
      continue;
    default:
      break;
    }
    break;
  }

  return Expr;
}
// Start refactor from this place.
std::unique_ptr<ASTNode> Parser::ParseMultiplicative() {
  auto Expr = ParseUnary();

  while (true) {
    const Token &Current = PeekNext();
    std::cout << "Multiplicative token " << TokenToString(Current.Type)
              << std::endl;
    switch (Current.Type) {
    case TokenType::STAR:
    case TokenType::SLASH:
    case TokenType::MOD:
      Expr = std::make_unique<ASTBinaryOperator>(Current.Type, std::move(Expr),
                                                 ParseUnary());
      continue;
    default:
      break;
    }
    break;
  }

  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParseUnary() {
  const Token PrefixUnaryOperator = PeekNext();

  switch (PrefixUnaryOperator.Type) {
  case TokenType::INC:
  case TokenType::DEC:
    return std::make_unique<ASTUnaryOperator>(
        ASTUnaryOperator::UnaryType::PREFIX, PrefixUnaryOperator.Type,
        ParsePostfix());
  default:
    // Rollback current token pointer because there's no unary operator.
    --CurrentBufferPtr;
    return ParsePostfix();
  }
}

std::unique_ptr<ASTNode> Parser::ParsePostfix() {
  auto Expr = ParsePrimary();

  while (true) {
    switch (const Token &Current = PeekNext(); Current.Type) {
    case TokenType::INC:
    case TokenType::DEC:
      Expr = std::make_unique<ASTUnaryOperator>(
          ASTUnaryOperator::UnaryType::POSTFIX, Current.Type, std::move(Expr));
      continue;
    default:
      break;
    }
    break;
  }

  return Expr;
}

std::unique_ptr<ASTNode> Parser::ParsePrimary() {
  switch (const Token &Current = PeekCurrent(); Current.Type) {
  case TokenType::SYMBOL:
    PeekNext();
    return std::make_unique<ASTSymbol>(Current.Data);
  default:
    return ParseConstant();
  }
}

const Token &Parser::PeekNext() {
  std::cout << "Peek: " << TokenToString((CurrentBufferPtr + 1)->Type)
            << std::endl;
  return *CurrentBufferPtr++;
}

const Token &Parser::PeekCurrent() const {
  std::cout << "Peek current: " << TokenToString(CurrentBufferPtr->Type)
            << std::endl;
  return *CurrentBufferPtr;
}

bool Parser::Match(const std::vector<TokenType> &Expected) {
  if (CurrentBufferPtr == BufferEnd) {
    DiagnosticError(CurrentBufferPtr->LineNo, CurrentBufferPtr->LineNo)
        << "End of buffer reached.";
    UnreachablePoint();
  }
  std::cout << "Match " << TokensToString(Expected) << std::endl;
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
  if (CurrentBufferPtr == BufferEnd) {
    DiagnosticError(CurrentBufferPtr->LineNo, CurrentBufferPtr->LineNo)
        << "End of buffer reached.";
    UnreachablePoint();
  }
  std::cout << "Require " << TokensToString(Expected) << std::endl;
  if (Match(Expected)) {
    /// Something from vector successfully matched and located in previous
    /// token.
    return *(CurrentBufferPtr - 1);
  }

  DiagnosticError(CurrentBufferPtr->LineNo, CurrentBufferPtr->ColumnNo)
      << "Expected " << TokensToString(Expected);
  UnreachablePoint();
}

const Token &Parser::Require(TokenType Expected) {
  return Require(std::vector<TokenType>{Expected});
}

} // namespace frontEnd
} // namespace weak