/* Diagnostic.hpp - Helper functions, used to emitting errors and warns.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_UTILITY_DIAGNOSTIC_HPP
#define WEAK_COMPILER_UTILITY_DIAGNOSTIC_HPP

#include <ostream>
#include <string>

namespace weak {

/// Simply terminate process.
[[noreturn]] void UnreachablePoint();

/// Print diagnostic message with WARN flag.
std::ostream &DiagnosticWarning();

/// Print diagnostic message with WARN flag.
std::ostream &DiagnosticWarning(unsigned LineNo, unsigned ColumnNo);

/// Print diagnostic message with ERROR flag and terminate program.
std::ostream &DiagnosticError();

/// Print diagnostic message with ERROR flag and terminate program.
std::ostream &DiagnosticError(unsigned LineNo, unsigned ColumnNo);

} // namespace weak

#endif // WEAK_COMPILER_UTILITY_DIAGNOSTIC_HPP