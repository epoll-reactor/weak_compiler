#include "Utility/Diagnostic.hpp"
#include <iostream>

namespace weak {

Diagnostic::Diagnostic(DiagType TheType) : Type(TheType) {
  EmitLabel();
}

Diagnostic::~Diagnostic() {
  std::cerr << std::endl;
  if (Type == DiagType::ERROR) {
    exit(-1);
  }
}

Diagnostic &Diagnostic::operator<<(const char *Data) {
  std::cerr << Data;
  return *this;
}

Diagnostic &Diagnostic::operator<<(std::string_view Data) {
  std::cerr << Data;
  return *this;
}

Diagnostic &Diagnostic::operator<<(signed int Data) {
  std::cerr << Data;
  return *this;
}

Diagnostic &Diagnostic::operator<<(unsigned int Data) {
  std::cerr << Data;
  return *this;
}

Diagnostic &Diagnostic::operator<<(signed long int Data) {
  std::cerr << Data;
  return *this;
}

Diagnostic &Diagnostic::operator<<(unsigned long int Data) {
  std::cerr << Data;
  return *this;
}

void Diagnostic::EmitLabel() const {
  if (Type == DiagType::ERROR) {
    std::cerr << "ERROR: ";
  } else {
    std::cerr << "WARN: ";
  }
}

Diagnostic DiagnosticWarning() {
  return Diagnostic{Diagnostic::DiagType::WARN};
}


Diagnostic DiagnosticError() {
  return Diagnostic{Diagnostic::DiagType::ERROR};
}

} // namespace weak