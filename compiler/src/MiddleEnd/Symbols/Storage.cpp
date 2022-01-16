/* Storage.cpp - Variable pool.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/Symbols/Storage.hpp"
#include "Utility/Diagnostic.hpp"
#include <algorithm>

using namespace weak::frontEnd;
using namespace weak::middleEnd;

namespace weak {
namespace middleEnd {

Storage::Storage() : CurrentScopeDepth(0U), CurrentAttribute(0U), Records() {}

void Storage::ScopeBegin() { ++CurrentScopeDepth; }

void Storage::ScopeEnd() {
  if (CurrentScopeDepth == 0) {
    DiagnosticError() << "No scopes left.";
    UnreachablePoint();
  }

  for (auto It = Records.begin(); It != Records.end();) {
    if (It->second.Depth == CurrentScopeDepth) {
      It = Records.erase(It);
    } else {
      ++It;
    }
  }
  --CurrentScopeDepth;
}

unsigned Storage::AddSymbol(std::string_view Name) {
  auto Found = std::find_if(Records.begin(), Records.end(),
                            [&Name](const std::pair<unsigned, Record> &R) {
                              return R.second.Name == Name;
                            });
  if (Found == Records.end()) {
    Record Variable{/*Depth=*/CurrentScopeDepth,
                    /*Attribute=*/CurrentAttribute,
                    /*Name=*/Name.data(),
                    /*DataType=*/TokenType::NONE,
                    /*StoredValue=*/0};
    unsigned SavedAttribute = CurrentAttribute;
    Records.emplace(CurrentAttribute++, std::move(Variable));
    return SavedAttribute;
  }

  const auto &[RecordAttribute, _] = *Found;
  return RecordAttribute;
}

Storage::Record *Storage::GetSymbol(unsigned Attribute) {
  auto Found = Records.find(Attribute);

  if (Found == Records.end() || Found->second.Depth > CurrentScopeDepth) {
    DiagnosticError() << "Variable not found.";
    UnreachablePoint();
  }

  return &Found->second;
}

Storage::Record *Storage::GetByName(std::string_view Name) {
  auto Found = std::find_if(Records.begin(), Records.end(),
                            [&Name](const std::pair<unsigned, Record> &R) {
                              return R.second.Name == Name;
                            });
  if (Found == Records.end()) {
    DiagnosticError() << "Variable not found: " << Name;
  }

  auto &[_, Variable] = *Found;
  return &Variable;
}

void Storage::SetSymbolType(unsigned Attribute, TokenType Type) {
  auto Found = Records.find(Attribute);
  if (Found == Records.end()) {
    DiagnosticError() << "Attempt to set type for variable that not exists.";
    UnreachablePoint();
  }
  Found->second.DataType = Type;
}

void Storage::SetVariableImpl(frontEnd::TokenType Type, unsigned int Attribute,
                              const AnyDataType &Value) {
  auto &[RecordAttribute, Variable] = FindByAttribute(Attribute);
  CheckIfVariableTypeIsSet(Variable);

  if (Variable.DataType != Type) {
    DiagnosticError() << "Type error: " << TokenToString(Type) << " expected.";
    UnreachablePoint();
  }

  Variable.Depth = CurrentScopeDepth;
  Variable.StoredValue = Value;
}

void Storage::setInstruction(unsigned Attribute, const Instruction &I) {
  auto &[RecordAttribute, Variable] = FindByAttribute(Attribute);
  CheckIfVariableTypeIsSet(Variable);

  /// Here we do not to check anything.

  Variable.Depth = CurrentScopeDepth;
  Variable.StoredValue = I;
}

void Storage::SetIntValue(unsigned Attribute, signed Value) {
  SetVariableImpl(TokenType::INTEGRAL_LITERAL, Attribute, Value);
}

void Storage::SetFloatValue(unsigned Attribute, float Value) {
  SetVariableImpl(TokenType::FLOATING_POINT_LITERAL, Attribute, Value);
}

void Storage::SetCharValue(unsigned Attribute, char Value) {
  SetVariableImpl(TokenType::CHAR, Attribute, Value);
}

void Storage::SetBoolValue(unsigned Attribute, bool Value) {
  SetVariableImpl(TokenType::BOOLEAN, Attribute, Value);
}

void Storage::SetStringValue(unsigned Attribute, std::string Value) {
  static_cast<void>(Attribute);
  static_cast<void>(Value);
}

void Storage::CheckIfVariableTypeIsSet(const Record &Variable) {
  if (Variable.DataType == TokenType::NONE) {
    weak::DiagnosticError()
        << "Internal error: type for " << Variable.Name << " is not set.";
    weak::UnreachablePoint();
  }
}

std::pair<const unsigned, Storage::Record> &
Storage::FindByAttribute(unsigned Attribute) {
  auto Found = std::find_if(Records.begin(), Records.end(),
                            [&Attribute](const std::pair<unsigned, Record> &R) {
                              return R.second.Attribute == Attribute;
                            });
  if (Found == Records.end()) {
    weak::DiagnosticError()
        << "Variable with attribute " << Attribute << " not found.";
    weak::UnreachablePoint();
  }
  return *Found;
}

} // namespace middleEnd
} // namespace weak