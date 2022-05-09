/* VariantOverload.hpp - Helper binding for std::visit.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */
#ifndef WEAK_COMPILER_UTILITY_VARIANT_OVERLOAD_HPP
#define WEAK_COMPILER_UTILITY_VARIANT_OVERLOAD_HPP

namespace weak {
namespace {

template <typename... Ts> struct Overload : Ts... { using Ts::operator()...; };
template <typename... Ts> Overload(Ts...) -> Overload<Ts...>;

} // namespace
} // namespace weak

#endif // WEAK_COMPILER_UTILITY_VARIANT_OVERLOAD_HPP
