#ifndef WEAK_COMPILER_MIDDLE_END_IR_REGISTERS_HPP
#define WEAK_COMPILER_MIDDLE_END_IR_REGISTERS_HPP

namespace weak {
namespace middleEnd {
/*! Common registers. */
enum struct Reg { REG_0, REG_1, REG_2, REG_3, REG_4, REG_5, REG_6, REG_7 };

const char *RegisterToString(Reg);

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_IR_REGISTERS_HPP
