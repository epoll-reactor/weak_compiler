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

  Diagnostic(enum DiagLevel TheLevel, unsigned TheLineNo, unsigned TheColumnNo)
      : Level(TheLevel), LineNo(TheLineNo), ColumnNo(TheColumnNo) {}

  Diagnostic(enum DiagLevel TheLevel)
      : Level(TheLevel), LineNo(0U), ColumnNo(0U) {}

  static void ClearErrBuf() { std::ostringstream().swap(ErrBuf); }

  void EmitLabel() {
    ErrBuf << ((Level == DiagLevel::ERROR) ? "ERROR" : "WARN");
    ErrBuf << " at line " << LineNo + 1 << ", column " << ColumnNo + 1 << ": ";
  }

  void EmitEmptyLabel() {
    ErrBuf << ((Level == DiagLevel::ERROR) ? "ERROR" : "WARN");
    ErrBuf << ": ";
  }

  unsigned LineNo;
  unsigned ColumnNo;
  static inline std::ostringstream ErrBuf;
};

void weak::UnreachablePoint(const char *Msg) {
  std::string ErrMsg = "Unreachable point reached: " + std::string(Msg) + '\n';
  throw std::runtime_error(ErrMsg);
}

weak::OstreamRAII::~OstreamRAII() noexcept(false) {
  std::string Buf = DiagImpl->ErrBuf.str();

  if (DiagImpl->Level == Diagnostic::DiagLevel::ERROR) {
    throw std::runtime_error(Buf);
  }

  std::cerr << Buf << std::endl;
}

std::ostream &weak::OstreamRAII::operator<<(const char *String) {
  return DiagImpl->ErrBuf << String;
}

static weak::OstreamRAII MakeMessage(Diagnostic::DiagLevel Level) {
  Diagnostic::ClearErrBuf();
  static Diagnostic Diag(Level);
  Diag.EmitEmptyLabel();
  return weak::OstreamRAII{&Diag};
}

static weak::OstreamRAII MakeMessage(Diagnostic::DiagLevel Level,
                                     unsigned LineNo, unsigned ColumnNo) {
  Diagnostic::ClearErrBuf();
  static Diagnostic Diag(Level, LineNo, ColumnNo);
  Diag.EmitLabel();
  return weak::OstreamRAII{&Diag};
}

weak::OstreamRAII weak::CompileWarning() {
  return MakeMessage(Diagnostic::DiagLevel::WARN);
}

weak::OstreamRAII weak::CompileWarning(unsigned LineNo, unsigned ColumnNo) {
  return MakeMessage(Diagnostic::DiagLevel::WARN, LineNo, ColumnNo);
}

weak::OstreamRAII weak::CompileError() {
  return MakeMessage(Diagnostic::DiagLevel::ERROR);
}

weak::OstreamRAII weak::CompileError(unsigned LineNo, unsigned ColumnNo) {
  return MakeMessage(Diagnostic::DiagLevel::ERROR, LineNo, ColumnNo);
}