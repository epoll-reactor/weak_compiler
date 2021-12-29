#ifndef WEAK_COMPILER_TESTS_FRONTEND_AST_MAKE_FUNCTIONS_HPP
#define WEAK_COMPILER_TESTS_FRONTEND_AST_MAKE_FUNCTIONS_HPP

#include "FrontEnd/AST/ASTBinaryOperator.hpp"
#include "FrontEnd/AST/ASTBooleanLiteral.hpp"
#include "FrontEnd/AST/ASTBreakStmt.hpp"
#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include "FrontEnd/AST/ASTContinueStmt.hpp"
#include "FrontEnd/AST/ASTDoWhileStmt.hpp"
#include "FrontEnd/AST/ASTFloatingPointLiteral.hpp"
#include "FrontEnd/AST/ASTForStmt.hpp"
#include "FrontEnd/AST/ASTIfStmt.hpp"
#include "FrontEnd/AST/ASTIntegerLiteral.hpp"
#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/AST/ASTReturnStmt.hpp"
#include "FrontEnd/AST/ASTStringLiteral.hpp"
#include "FrontEnd/AST/ASTUnaryOperator.hpp"
#include "FrontEnd/AST/ASTVarDecl.hpp"
#include "FrontEnd/AST/ASTWhileStmt.hpp"

using namespace weak::frontEnd;

static std::unique_ptr<ASTNode> MakeBreak() {
  return std::make_unique<ASTBreakStmt>();
}

static std::unique_ptr<ASTNode> MakeContinue() {
  return std::make_unique<ASTContinueStmt>();
}

static std::unique_ptr<ASTNode> MakeReturn(std::unique_ptr<ASTNode> &&Operand) {
  return std::make_unique<ASTReturnStmt>(std::move(Operand));
}

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

static std::unique_ptr<ASTCompoundStmt>
MakeCompound(std::vector<std::unique_ptr<ASTNode>> &&Nodes) {
  return std::make_unique<ASTCompoundStmt>(std::move(Nodes));
}

static std::unique_ptr<ASTNode>
MakeIf(std::unique_ptr<ASTNode> &&Condition,
       std::unique_ptr<ASTCompoundStmt> &&ThenBody,
       std::unique_ptr<ASTCompoundStmt> &&ElseBody) {
  return std::make_unique<ASTIfStmt>(std::move(Condition), std::move(ThenBody),
                                     std::move(ElseBody));
}

static std::unique_ptr<ASTNode>
MakeFor(std::unique_ptr<ASTNode> &&Init, std::unique_ptr<ASTNode> &&Condition,
        std::unique_ptr<ASTNode> &&Increment,
        std::unique_ptr<ASTCompoundStmt> &&TheBody) {
  return std::make_unique<ASTForStmt>(std::move(Init), std::move(Condition),
                                      std::move(Increment), std::move(TheBody));
}

static std::unique_ptr<ASTNode>
MakeWhile(std::unique_ptr<ASTNode> &&Condition,
          std::unique_ptr<ASTCompoundStmt> &&Body) {
  return std::make_unique<ASTWhileStmt>(std::move(Condition), std::move(Body));
}

static std::unique_ptr<ASTNode>
MakeDoWhile(std::unique_ptr<ASTCompoundStmt> &&Body,
            std::unique_ptr<ASTNode> &&Condition) {
  return std::make_unique<ASTDoWhileStmt>(std::move(Body),
                                          std::move(Condition));
}

#endif // WEAK_COMPILER_TESTS_FRONTEND_AST_MAKE_FUNCTIONS_HPP