/* Diagnostic.hpp - Helper functions, used to emitting errors and warns.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_UTILITY_DIAGNOSTIC_HPP
#define WEAK_COMPILER_UTILITY_DIAGNOSTIC_HPP

#include <ostream>

namespace weak {

/// This requires the string as first argument in diagnostic messages.
struct OstreamRAII {
  ~OstreamRAII();
  std::ostream &operator<<(const char *);
};

/// Simply terminate process.
[[noreturn]] void UnreachablePoint();

/// Print diagnostic message with WARN flag.
OstreamRAII DiagnosticWarning();

/// Print diagnostic message with WARN flag.
OstreamRAII DiagnosticWarning(unsigned LineNo, unsigned ColumnNo);

/// Print diagnostic message with ERROR flag and terminate program.
OstreamRAII DiagnosticError();

/// Print diagnostic message with ERROR flag and terminate program.
OstreamRAII DiagnosticError(unsigned LineNo, unsigned ColumnNo);

} // namespace weak

#endif // WEAK_COMPILER_UTILITY_DIAGNOSTIC_HPP