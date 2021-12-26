#ifndef WEAK_COMPILER_DIAGNOSTIC_HPP
#define WEAK_COMPILER_DIAGNOSTIC_HPP

#include <string>

namespace weak {

class Diagnostic {
public:
  enum struct DiagType { WARN, ERROR } Type;

  Diagnostic(DiagType);
  ~Diagnostic();

  Diagnostic() = delete;
  Diagnostic(const Diagnostic&) = delete;
  Diagnostic(Diagnostic&&) = delete;
  Diagnostic& operator=(Diagnostic&&) = delete;
  Diagnostic& operator=(const Diagnostic&) = delete;

  Diagnostic &operator<<(const char *);
  Diagnostic &operator<<(std::string_view);
  Diagnostic &operator<<(signed int);
  Diagnostic &operator<<(unsigned int);
  Diagnostic &operator<<(signed long int);
  Diagnostic &operator<<(unsigned long int);

private:
  void EmitLabel() const;
};

Diagnostic DiagnosticWarning();
Diagnostic DiagnosticError();

} // namespace weak

#endif // WEAK_COMPILER_DIAGNOSTIC_HPP
