/* Registers.cpp - List of IR registers.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/IR/Registers.hpp"

namespace weak {

const char *middleEnd::RegisterToString(Reg Reg) {
  switch (Reg) {
  case Reg::REG_0:
    return "reg_0";
  case Reg::REG_1:
    return "reg_1";
  case Reg::REG_2:
    return "reg_2";
  case Reg::REG_3:
    return "reg_3";
  case Reg::REG_4:
    return "reg_4";
  case Reg::REG_5:
    return "reg_5";
  case Reg::REG_6:
    return "reg_6";
  case Reg::REG_7:
    return "reg_7";
  }
  return "";
}

} // namespace weak