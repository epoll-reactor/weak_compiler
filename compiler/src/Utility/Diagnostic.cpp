#include "Utility/Diagnostic.hpp"
#include <iostream>

void weak::UnreachablePoint() { exit(-1); }

namespace weak {

Diagnostic::Diagnostic(DiagLevel TheType, unsigned TheLineNo,
                       unsigned TheColumnNo)
    : Level(TheType), LineNo(TheLineNo), ColumnNo(TheColumnNo) {
  EmitLabel();
}

Diagnostic::~Diagnostic() {
  std::cerr << std::endl;
  if (Level == DiagLevel::ERROR) {
    exit(-1);
  }
}

const Diagnostic &Diagnostic::operator<<(const char *Data) const {
  std::cerr << Data;
  return *this;
}

const Diagnostic &Diagnostic::operator<<(std::string_view Data) const {
  std::cerr << Data;
  return *this;
}

const Diagnostic &Diagnostic::operator<<(signed int Data) const {
  std::cerr << Data;
  return *this;
}

const Diagnostic &Diagnostic::operator<<(unsigned int Data) const {
  std::cerr << Data;
  return *this;
}

const Diagnostic &Diagnostic::operator<<(signed long int Data) const {
  std::cerr << Data;
  return *this;
}

const Diagnostic &Diagnostic::operator<<(unsigned long int Data) const {
  std::cerr << Data;
  return *this;
}

void Diagnostic::EmitLabel() const {
  if (Level == DiagLevel::ERROR) {
    std::cerr << "ERROR - at line " << LineNo + 1 << ", column " << ColumnNo + 1
              << ": ";
  } else {
    std::cerr << "WARN - at line " << LineNo + 1 << ", column " << ColumnNo + 1
              << ": ";
  }
}

Diagnostic DiagnosticWarning(unsigned LineNo, unsigned ColumnNo) {
  return Diagnostic(Diagnostic::DiagLevel::WARN, LineNo, ColumnNo);
}

Diagnostic DiagnosticError(unsigned LineNo, unsigned ColumnNo) {
  return Diagnostic(Diagnostic::DiagLevel::ERROR, LineNo, ColumnNo);
}

} // namespace weak
