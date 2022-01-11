#ifndef WEAK_COMPILER_MIDDLE_END_IR_OPERATIONS_HPP
#define WEAK_COMPILER_MIDDLE_END_IR_OPERATIONS_HPP

namespace weak {
namespace middleEnd {
/*! All accepted by IR operations. */
enum struct Op {
  MOV,
  ADD,
  SUB,
  MUL,
  DIV,
  AND,
  OR,
  SHL,
  SHR,
  CMPLT,
  CMPGT,
  CMPLE,
  CMPGE,
  CMPEQ,
  CMPNEQ,
  PUSH,
  POP,
  INC,
  DEC,
  JMP
};

const char *OperationToString(Op);

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_IR_OPERATIONS_HPP
