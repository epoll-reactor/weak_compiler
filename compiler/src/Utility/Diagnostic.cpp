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
    std::cerr << ((Level == DiagLevel::ERROR) ? "ERROR" : "WARN");
    std::cerr << " at line " << LineNo + 1 << ", column " << ColumnNo + 1
              << ": ";
  }

  void EmitEmptyLabel() {
    std::cerr << ((Level == DiagLevel::ERROR) ? "ERROR" : "WARN");
    std::cerr << ": ";
  }
};

} // namespace

void weak::UnreachablePoint() { exit(-1); }

weak::OstreamRAII::~OstreamRAII() {
  if (Error) {
    exit(-1);
  }
  std::cerr << std::endl;
}

std::ostream &weak::OstreamRAII::operator<<(const char *String) {
  return std::cerr << String;
}

weak::OstreamRAII weak::CompileWarning() {
  [[maybe_unused]] Diagnostic _(Diagnostic::DiagLevel::WARN);
  return OstreamRAII{OstreamRAII::Ok};
}

weak::OstreamRAII weak::CompileWarning(unsigned LineNo, unsigned ColumnNo) {
  [[maybe_unused]] Diagnostic _(Diagnostic::DiagLevel::WARN, LineNo, ColumnNo);
  return OstreamRAII{OstreamRAII::Ok};
}

weak::OstreamRAII weak::CompileError() {
  [[maybe_unused]] Diagnostic _(Diagnostic::DiagLevel::ERROR);
  return OstreamRAII{OstreamRAII::Error};
}

weak::OstreamRAII weak::CompileError(unsigned LineNo, unsigned ColumnNo) {
  [[maybe_unused]] Diagnostic _(Diagnostic::DiagLevel::ERROR, LineNo, ColumnNo);
  return OstreamRAII{OstreamRAII::Error};
}