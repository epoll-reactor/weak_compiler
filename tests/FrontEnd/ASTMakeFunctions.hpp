#ifndef WEAK_COMPILER_TESTS_FRONTEND_AST_MAKE_FUNCTIONS_HPP
#define WEAK_COMPILER_TESTS_FRONTEND_AST_MAKE_FUNCTIONS_HPP

#include "FrontEnd/AST/ASTBinaryOperator.hpp"
#include "FrontEnd/AST/ASTBooleanLiteral.hpp"
#include "FrontEnd/AST/ASTFloatingPointLiteral.hpp"
#include "FrontEnd/AST/ASTIntegerLiteral.hpp"
#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/AST/ASTStringLiteral.hpp"
#include "FrontEnd/AST/ASTUnaryOperator.hpp"

using namespace weak::frontEnd;

static std::unique_ptr<ASTNode> MakeBoolean(bool Value) {
  return std::make_unique<ASTBooleanLiteral>(Value);
}

static std::unique_ptr<ASTNode> MakeFloat(double Value) {
  return std::make_unique<ASTFloatingPointLiteral>(Value);
}

static std::unique_ptr<ASTNode> MakeInteger(signed Value) {
  return std::make_unique<ASTIntegerLiteral>(Value);
}

static std::unique_ptr<ASTNode> MakeString(std::string Value) {
  return std::make_unique<ASTStringLiteral>(std::move(Value));
}

static std::unique_ptr<ASTNode> MakeBinary(TokenType Type,
                                           std::unique_ptr<ASTNode> &&LHS,
                                           std::unique_ptr<ASTNode> &&RHS) {
  return std::make_unique<ASTBinaryOperator>(Type, std::move(LHS),
                                             std::move(RHS));
}

static std::unique_ptr<ASTNode> MakeUnary(ASTUnaryOperator::UnaryType Type,
                                          TokenType Operation,
                                          std::unique_ptr<ASTNode> &&Operand) {
  return std::make_unique<ASTUnaryOperator>(Type, Operation,
                                            std::move(Operand));
}

#endif // WEAK_COMPILER_TESTS_FRONTEND_AST_MAKE_FUNCTIONS_HPP
