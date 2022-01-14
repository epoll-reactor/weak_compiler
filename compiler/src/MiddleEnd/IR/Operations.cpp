/* Operations.cpp - List of IR operations.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/IR/Operations.hpp"

namespace weak {

const char *middleEnd::OperationToString(Op Op) {
  switch (Op) {
  case Op::MOV:
    return "mov";
  case Op::ADD:
    return "add";
  case Op::SUB:
    return "sub";
  case Op::MUL:
    return "mul";
  case Op::DIV:
    return "div";
  case Op::AND:
    return "and";
  case Op::OR:
    return "or";
  case Op::SHL:
    return "shl";
  case Op::SHR:
    return "shr";
  case Op::CMPLT:
    return "cmplt";
  case Op::CMPGT:
    return "cmpgt";
  case Op::CMPLE:
    return "cmple";
  case Op::CMPGE:
    return "cmpge";
  case Op::CMPEQ:
    return "cmpeq";
  case Op::CMPNEQ:
    return "cmpneq";
  case Op::PUSH:
    return "push";
  case Op::POP:
    return "pop";
  case Op::INC:
    return "inc";
  case Op::DEC:
    return "dec";
  case Op::JMP:
    return "jmp";
  }
  return "";
}

} // namespace weak