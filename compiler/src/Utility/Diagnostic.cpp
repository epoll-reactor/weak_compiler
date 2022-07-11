/* Diagnostic.cpp - Helper functions, used to emitting errors and warns.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "Utility/Diagnostic.hpp"

#include <iostream>
#include <sstream>

class Diagnostic {
public:
  enum struct DiagLevel { WARN, ERROR } const Level;

  Diagnostic(enum DiagLevel TheLevel, unsigned LineNo, unsigned ColumnNo)
      : Level(TheLevel) {
    EmitLabel(LineNo, ColumnNo);
  }

  Diagnostic(enum DiagLevel TheLevel) : Level(TheLevel) { EmitEmptyLabel(); }

  static void ClearErrBuf() { std::ostringstream().swap(ErrBuf); }

private:
  friend struct weak::OstreamRAII;

  void EmitLabel(unsigned LineNo, unsigned ColumnNo) {
    ErrBuf << ((Level == DiagLevel::ERROR) ? "ERROR" : "WARN");
    ErrBuf << " at line " << LineNo + 1 << ", column " << ColumnNo + 1 << ": ";
  }

  void EmitEmptyLabel() {
    ErrBuf << ((Level == DiagLevel::ERROR) ? "ERROR" : "WARN");
    ErrBuf << ": ";
  }

  static inline std::ostringstream ErrBuf;
};

void weak::UnreachablePoint(const char *Msg) {
  std::string ErrMsg = "Unreachable point reached: " + std::string(Msg) + '\n';
  throw std::runtime_error(ErrMsg);
}

weak::OstreamRAII::~OstreamRAII() noexcept(false) {
  if (DiagImpl->Level == Diagnostic::DiagLevel::ERROR) {
    throw std::runtime_error(DiagImpl->ErrBuf.str());
  } else {
    std::cerr << DiagImpl->ErrBuf.str() << std::endl;
  }
}

std::ostream &weak::OstreamRAII::operator<<(const char *String) {
  return DiagImpl->ErrBuf << String;
}

weak::OstreamRAII weak::CompileWarning() {
  Diagnostic::ClearErrBuf();
  Diagnostic _(::Diagnostic::DiagLevel::WARN);
  return OstreamRAII{&_};
}

weak::OstreamRAII weak::CompileWarning(unsigned LineNo, unsigned ColumnNo) {
  Diagnostic::ClearErrBuf();
  Diagnostic _(Diagnostic::DiagLevel::WARN, LineNo, ColumnNo);
  return OstreamRAII{&_};
}

weak::OstreamRAII weak::CompileError() {
  Diagnostic::ClearErrBuf();
  Diagnostic _(Diagnostic::DiagLevel::ERROR);
  return OstreamRAII{&_};
}

weak::OstreamRAII weak::CompileError(unsigned LineNo, unsigned ColumnNo) {
  Diagnostic::ClearErrBuf();
  Diagnostic _(Diagnostic::DiagLevel::ERROR, LineNo, ColumnNo);
  return OstreamRAII{&_};
}