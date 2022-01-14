/* CodeGen.hpp - Intermediate code generator.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_CODE_GEN_HPP
#define WEAK_COMPILER_MIDDLE_END_CODE_GEN_HPP

#include "FrontEnd/AST/ASTNode.hpp"
#include "MiddleEnd/CodeGen/CodeEmitter.hpp"
#include "MiddleEnd/IR/Instruction.hpp"

namespace weak {
namespace frontEnd {

class ASTNode;
class ASTBinaryOperator;
class ASTBooleanLiteral;
class ASTBreakStmt;
class ASTCompoundStmt;
class ASTContinueStmt;
class ASTDoWhileStmt;
class ASTFloatingPointLiteral;
class ASTForStmt;
class ASTFunctionDecl;
class ASTFunctionCall;
class ASTIfStmt;
class ASTIntegerLiteral;
class ASTReturnStmt;
class ASTStringLiteral;
class ASTSymbol;
class ASTUnaryOperator;
class ASTVarDecl;
class ASTWhileStmt;

} // namespace frontEnd
} // namespace weak

namespace weak {
namespace middleEnd {

class CodeGen {
public:
  using AnyInstruction = std::variant<
      /* If statement produces another instruction. */ Instruction,
      /* If statement produces digit. */ signed>;

  CodeGen(frontEnd::ASTNode *TheRootNode);

  void CreateCode();

private:
  AnyInstruction VisitBaseNode(const frontEnd::ASTNode *);
  AnyInstruction Visit(const frontEnd::ASTBinaryOperator *);
  AnyInstruction Visit(const frontEnd::ASTBooleanLiteral *);
  AnyInstruction Visit(const frontEnd::ASTBreakStmt *);
  AnyInstruction Visit(const frontEnd::ASTCompoundStmt *);
  AnyInstruction Visit(const frontEnd::ASTContinueStmt *);
  AnyInstruction Visit(const frontEnd::ASTDoWhileStmt *);
  AnyInstruction Visit(const frontEnd::ASTFloatingPointLiteral *);
  AnyInstruction Visit(const frontEnd::ASTForStmt *);
  AnyInstruction Visit(const frontEnd::ASTFunctionDecl *);
  AnyInstruction Visit(const frontEnd::ASTFunctionCall *);
  AnyInstruction Visit(const frontEnd::ASTIfStmt *);
  AnyInstruction Visit(const frontEnd::ASTIntegerLiteral *);
  AnyInstruction Visit(const frontEnd::ASTReturnStmt *);
  AnyInstruction Visit(const frontEnd::ASTStringLiteral *);
  AnyInstruction Visit(const frontEnd::ASTSymbol *);
  AnyInstruction Visit(const frontEnd::ASTUnaryOperator *);
  AnyInstruction Visit(const frontEnd::ASTVarDecl *);
  AnyInstruction Visit(const frontEnd::ASTWhileStmt *);

  frontEnd::ASTNode *RootNode;

  CodeEmitter Emitter;

  std::vector<Instruction> Instructions;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_CODE_GEN_HPP
