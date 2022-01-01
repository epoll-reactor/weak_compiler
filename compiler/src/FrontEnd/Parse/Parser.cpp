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

const Token &Parser::ParseConstant() {
  switch (const auto &Current = PeekCurrent(); Current.Type) {
  case TokenType::INTEGRAL_LITERAL:
  case TokenType::FLOATING_POINT_LITERAL:
  case TokenType::STRING_LITERAL:
  case TokenType::BOOLEAN: // Fall through.
    return Current;
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
  }

  Require(TokenType::CLOSE_CURLY_BRACKET);

  return std::make_unique<ASTCompoundStmt>(std::move(Statements));
}

std::unique_ptr<ASTNode> Parser::ParseStatement() {
  return std::unique_ptr<ASTNode>();
}

const Token &Parser::PeekNext() { return *CurrentBufferPtr++; }

const Token &Parser::PeekCurrent() const { return *CurrentBufferPtr; }

bool Parser::Match(const std::vector<TokenType> &Expected) {
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