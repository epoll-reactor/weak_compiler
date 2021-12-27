#ifndef WEAK_COMPILER_UTILITY_DIAGNOSTIC_HPP
#define WEAK_COMPILER_UTILITY_DIAGNOSTIC_HPP

#include <string>

namespace weak {

[[noreturn]] void UnreachablePoint();

class Diagnostic {
public:
  enum struct DiagType { WARN, ERROR } const Type;

  Diagnostic(DiagType TheType, unsigned TheLineNo, unsigned TheColumnNo);
  ~Diagnostic();

  Diagnostic() = delete;
  Diagnostic(const Diagnostic &) = delete;
  Diagnostic(Diagnostic &&) = delete;
  Diagnostic &operator=(Diagnostic &&) = delete;
  Diagnostic &operator=(const Diagnostic &) = delete;

  Diagnostic &operator<<(const char *);
  Diagnostic &operator<<(std::string_view);
  Diagnostic &operator<<(signed int);
  Diagnostic &operator<<(unsigned int);
  Diagnostic &operator<<(signed long int);
  Diagnostic &operator<<(unsigned long int);

private:
  /// Put label with error location (line and column) to stderr
  void EmitLabel() const;

  /// Line number (used for error generating).
  unsigned LineNo;

  /// Column number (used for error generating).
  unsigned ColumnNo;
};

Diagnostic DiagnosticWarning(unsigned LineNo, unsigned ColumnNo);
Diagnostic DiagnosticError(unsigned LineNo, unsigned ColumnNo);

} // namespace weak

#endif // WEAK_COMPILER_UTILITY_DIAGNOSTIC_HPP
