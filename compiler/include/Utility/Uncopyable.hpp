/* Uncopyable.hpp - Class to forbid copying.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_UTILITY_UNCOPYABLE_HPP
#define WEAK_COMPILER_UTILITY_UNCOPYABLE_HPP

namespace weak {
/// Should be used by inheritance to disable object copying semantics and
/// prevent boilerplate code.
class Uncopyable {
public:
  Uncopyable() = default;
  Uncopyable(Uncopyable &&) = default;
  Uncopyable &operator=(Uncopyable &&) = default;
  Uncopyable(const Uncopyable &) = delete;
  Uncopyable &operator=(const Uncopyable &) = delete;
};

} // namespace weak

#endif // WEAK_COMPILER_UTILITY_UNCOPYABLE_HPP
