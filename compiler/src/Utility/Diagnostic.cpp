#include "Utility/Diagnostic.hpp"
#include <iostream>

void weak::UnreachablePoint() { exit(-1); }

namespace weak {

Diagnostic::Diagnostic(DiagType TheType, unsigned TheLineNo,
                       unsigned TheColumnNo)
    : Type(TheType), LineNo(TheLineNo), ColumnNo(TheColumnNo) {
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
    std::cerr << "ERROR - at line " << LineNo + 1 << ", column " << ColumnNo + 1
              << ": ";
  } else {
    std::cerr << "WARN - at line " << LineNo + 1 << ", column " << ColumnNo + 1
              << ": ";
  }
}

Diagnostic DiagnosticWarning(unsigned LineNo, unsigned ColumnNo) {
  return Diagnostic(Diagnostic::DiagType::WARN, LineNo, ColumnNo);
}

Diagnostic DiagnosticError(unsigned LineNo, unsigned ColumnNo) {
  return Diagnostic(Diagnostic::DiagType::ERROR, LineNo, ColumnNo);
}

} // namespace weak