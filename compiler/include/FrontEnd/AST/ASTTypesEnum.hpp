#ifndef WEAK_COMPILER_FRONTEND_AST_AST_TYPES_ENUM_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_TYPES_ENUM_HPP

namespace weak {
namespace frontEnd {

enum struct ASTType {
  // Abstract node.
  BASE_NODE,

  // Literals.
  INTEGRAL_LITERAL,
  FLOATING_POINT_LITERAL,
  STRING_LITERAL,
  BOOLEAN_LITERAL,

  // Assignments (e.g int Value = 0).
  INT_DECL,
  CHAR_DECL,
  STRING_DECL,
  BOOLEAN_DECL,

  // Return type.
  VOID_DECL,

  // Function parameter (e.g int Value).
  PARAMETER,

  // Iteration statements.
  BREAK_STMT,
  CONTINUE_STMT,

  // Operand-Operator-Operand (also with [] operator).
  BINARY,

  // Operand-Operator.
  PREFIX_UNARY,
  POSTFIX_UNARY,

  // Branches.
  IF_STMT,

  // Loops.
  FOR_STMT,
  WHILE_STMT,
  DO_WHILE_STMT,

  // Jump statements.
  RETURN_STMT
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_TYPES_ENUM_HPP
