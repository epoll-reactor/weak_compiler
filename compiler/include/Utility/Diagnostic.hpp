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

  friend Diagnostic DiagnosticWarning(unsigned LineNo, unsigned ColumnNo);
  friend Diagnostic DiagnosticError(unsigned LineNo, unsigned ColumnNo);

  /// Put label with error location (line and column) to stderr.
  void EmitLabel() const;

  /// The line number displayed in the error/warn message.
  unsigned LineNo;

  /// The column number displayed in the error/warn message.
  unsigned ColumnNo;
};

/// Print diagnostic message with WARN flag.
Diagnostic DiagnosticWarning(unsigned LineNo, unsigned ColumnNo);

/// Print diagnostic message with ERROR flag and terminate program.
Diagnostic DiagnosticError(unsigned LineNo, unsigned ColumnNo);

} // namespace weak

#endif // WEAK_COMPILER_UTILITY_DIAGNOSTIC_HPP
