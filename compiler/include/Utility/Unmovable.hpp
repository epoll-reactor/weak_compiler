/* Unmovable.hpp - Class to forbid moving.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_UTILITY_UNMOVABLE_HPP
#define WEAK_COMPILER_UTILITY_UNMOVABLE_HPP

namespace weak {
/// Should be used by inheritance to disable object moving semantics and prevent
/// boilerplate code.
class Unmovable {
public:
  Unmovable() = default;
  Unmovable(Unmovable &&) = delete;
  Unmovable &operator=(Unmovable &&) = delete;
  Unmovable(const Unmovable &) = default;
  Unmovable &operator=(const Unmovable &) = default;
};

} // namespace weak

#endif // WEAK_COMPILER_UTILITY_UNMOVABLE_HPP
