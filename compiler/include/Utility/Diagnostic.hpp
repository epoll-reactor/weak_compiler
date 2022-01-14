/* Diagnostic.hpp - helper functions, used to emitting errors and warns.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_UTILITY_DIAGNOSTIC_HPP
#define WEAK_COMPILER_UTILITY_DIAGNOSTIC_HPP

#include <string>

namespace weak {

/// Simply terminate process.
[[noreturn]] void UnreachablePoint();

/// Interface used to construct and print notes when analyzing input data.
class Diagnostic {
public:
  ~Diagnostic();

  Diagnostic() = delete;
  Diagnostic(const Diagnostic &) = delete;
  Diagnostic(Diagnostic &&) = delete;
  Diagnostic &operator=(Diagnostic &&) = delete;
  Diagnostic &operator=(const Diagnostic &) = delete;

  const Diagnostic &operator<<(const char *) const;
  const Diagnostic &operator<<(std::string_view) const;
  const Diagnostic &operator<<(signed int) const;
  const Diagnostic &operator<<(unsigned int) const;
  const Diagnostic &operator<<(signed long int) const;
  const Diagnostic &operator<<(unsigned long int) const;

private:
  enum struct DiagLevel { WARN, ERROR } const Level;

  /// Setup diagnostic level and display prompt.
  Diagnostic(DiagLevel TheType, unsigned TheLineNo, unsigned TheColumnNo);

  /// Setup diagnostic level and display prompt without line/column position.
  Diagnostic(DiagLevel TheType);

  friend Diagnostic DiagnosticWarning();
  friend Diagnostic DiagnosticWarning(unsigned LineNo, unsigned ColumnNo);
  friend Diagnostic DiagnosticError();
  friend Diagnostic DiagnosticError(unsigned LineNo, unsigned ColumnNo);

  /// Put label with line/column location to stderr.
  void EmitLabel() const;

  /// Put label without line/column location to stderr.
  void EmitEmptyLabel() const;

  /// The line number displayed in the error/warn message.
  unsigned LineNo;

  /// The column number displayed in the error/warn message.
  unsigned ColumnNo;
};

/// Print diagnostic message with WARN flag.
Diagnostic DiagnosticWarning();

/// Print diagnostic message with WARN flag.
Diagnostic DiagnosticWarning(unsigned LineNo, unsigned ColumnNo);

/// Print diagnostic message with ERROR flag and terminate program.
Diagnostic DiagnosticError();

/// Print diagnostic message with ERROR flag and terminate program.
Diagnostic DiagnosticError(unsigned LineNo, unsigned ColumnNo);

} // namespace weak

#endif // WEAK_COMPILER_UTILITY_DIAGNOSTIC_HPP
