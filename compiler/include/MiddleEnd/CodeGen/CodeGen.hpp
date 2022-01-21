/* CodeGen.hpp - Intermediate code generator.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_CODE_GEN_HPP
#define WEAK_COMPILER_MIDDLE_END_CODE_GEN_HPP

#include "FrontEnd/AST/ASTVisitor.hpp"
#include "MiddleEnd/CodeGen/CodeEmitter.hpp"
#include "MiddleEnd/Symbols/Storage.hpp"

namespace weak {
namespace middleEnd {

/*!
 * Code generator.
 *
 * Transforms an AST to intermediate three-address code.
 */
class CodeGen : private frontEnd::ASTVisitor {
public:
  using AnyInstruction = std::variant<
    /* If statement produces another instruction. */ Instruction,
    /* If statement produces unary instruction. */ UnaryInstruction,
    /* If statement produces reference. */ Reference,
    /* If statement produces digit. */ signed,
    /* If statement produces float. */ double,
    /* If statement produces boolean. */ bool>;

  CodeGen(Storage *TheVariablePool, frontEnd::ASTNode *TheRootNode);

  /// Generates a sequence of instructions beginning from the root AST.
  const std::list<weak::middleEnd::AnyInstruction> &CreateCode();

private:
  void Visit(const frontEnd::ASTBinaryOperator *) const override;
  void Visit(const frontEnd::ASTBooleanLiteral *) const override;
  void Visit(const frontEnd::ASTBreakStmt *) const override;
  void Visit(const frontEnd::ASTCompoundStmt *) const override;
  void Visit(const frontEnd::ASTContinueStmt *) const override;
  void Visit(const frontEnd::ASTDoWhileStmt *) const override;
  void Visit(const frontEnd::ASTFloatingPointLiteral *) const override;
  void Visit(const frontEnd::ASTForStmt *) const override;
  void Visit(const frontEnd::ASTFunctionDecl *) const override;
  void Visit(const frontEnd::ASTFunctionCall *) const override;
  void Visit(const frontEnd::ASTIfStmt *) const override;
  void Visit(const frontEnd::ASTIntegerLiteral *) const override;
  void Visit(const frontEnd::ASTReturnStmt *) const override;
  void Visit(const frontEnd::ASTStringLiteral *) const override;
  void Visit(const frontEnd::ASTSymbol *) const override;
  void Visit(const frontEnd::ASTUnaryOperator *) const override;
  void Visit(const frontEnd::ASTVarDecl *) const override;
  void Visit(const frontEnd::ASTWhileStmt *) const override;

  /*!
   * Emit if instruction with empty jump label to be installed later.
   *
   * \tparam DataType - There is a specialisations for AST literals
   * (ASTIntegerLiteral, ASTBooleanLiteral and ASTFloatingPointLiteral) only,
   * for all other types calling this function will cause linking error.
   */
  template <typename DataType>
  IfInstruction *CreateUnaryCondition(const frontEnd::ASTNode *) const;

  /*!
   * Called if binary statement represents variable assignment, called from
   * Visit function for the ASTBinaryOperator.
   */
  void EmitAssignment(const frontEnd::ASTBinaryOperator *) const;

  /*!
   * Create conditional instruction with empty jump label to be installed later.
   *
   * Represents a CreateUnaryCondition for AST literals, but for a
   * ASTBinaryOperator this erases last instruction from internal Emitter list,
   * and next returns conditional instruction.
   */
  IfInstruction *EmitCondition(const frontEnd::ASTNode *) const;

  /*!
   * Emits a sequence of instructions with checking for `break` and `continue`
   * statements.
   */
  void EmitLoopBody(Jump *&BreakJump, Jump *&ContinueJump,
                    const frontEnd::ASTCompoundStmt *Body) const;

  frontEnd::ASTNode *RootNode;

  mutable CodeEmitter Emitter;

  mutable AnyInstruction LastInstruction;

  mutable unsigned CurrentGotoLabel;

  mutable Storage *VariablePool;

  mutable bool LoopHasBreak;
  mutable bool LoopHasContinue;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_CODE_GEN_HPP
