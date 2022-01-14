/* Diagnostic.cpp - Helper functions, used to emitting errors and warns.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "Utility/Diagnostic.hpp"
#include <iostream>

namespace {

class Diagnostic {
public:
  enum struct DiagLevel { WARN, ERROR } const Level;

  Diagnostic(enum DiagLevel TheLevel, unsigned LineNo, unsigned ColumnNo)
      : Level(TheLevel) {
    EmitLabel(LineNo, ColumnNo);
  }

  Diagnostic(enum DiagLevel TheLevel) : Level(TheLevel) { EmitEmptyLabel(); }

private:
  void EmitLabel(unsigned LineNo, unsigned ColumnNo) {
    if (Level == DiagLevel::ERROR) {
      std::cerr << "ERROR - at line " << LineNo + 1 << ", column "
                << ColumnNo + 1 << ": ";
    } else {
      std::cerr << "WARN - at line " << LineNo + 1 << ", column "
                << ColumnNo + 1 << ": ";
    }
  }

  void EmitEmptyLabel() {
    if (Level == DiagLevel::ERROR) {
      std::cerr << "ERROR : ";
    } else {
      std::cerr << "WARN: ";
    }
  }
};

} // namespace

void weak::UnreachablePoint() { exit(-1); }

std::ostream &weak::DiagnosticWarning() {
  [[maybe_unused]] Diagnostic _(Diagnostic::DiagLevel::WARN);
  return std::cerr;
}

std::ostream &weak::DiagnosticWarning(unsigned LineNo, unsigned ColumnNo) {
  [[maybe_unused]] Diagnostic _(Diagnostic::DiagLevel::WARN, LineNo, ColumnNo);
  return std::cerr;
}

std::ostream &weak::DiagnosticError() {
  [[maybe_unused]] Diagnostic _(Diagnostic::DiagLevel::ERROR);
  return std::cerr;
}

std::ostream &weak::DiagnosticError(unsigned LineNo, unsigned ColumnNo) {
  [[maybe_unused]] Diagnostic _(Diagnostic::DiagLevel::ERROR, LineNo, ColumnNo);
  return std::cerr;
}