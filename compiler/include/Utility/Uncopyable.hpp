#ifndef WEAK_COMPILER_UTILITY_UNCOPYABLE_HPP
#define WEAK_COMPILER_UTILITY_UNCOPYABLE_HPP

namespace weak {
/*!
 * Should be used by inheritance to disable object copying semantics and prevent
 * boilerplate code.
 */
class Uncopyable {
public:
  Uncopyable() = default;
  Uncopyable(const Uncopyable &) = delete;
  Uncopyable &operator=(const Uncopyable &) = delete;
};

} // namespace weak

#endif // WEAK_COMPILER_UTILITY_UNCOPYABLE_HPP
