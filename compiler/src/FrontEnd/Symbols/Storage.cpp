#include "FrontEnd/Symbols/Storage.hpp"
#include "Utility/Diagnostic.hpp"
#include <algorithm>

using namespace weak::frontEnd;

static auto &FindByAttribute(Storage::RecordMap &RecordMap,
                             unsigned Attribute) {
  using StorageRecord = Storage::StorageRecord;
  auto Found =
      std::find_if(RecordMap.begin(), RecordMap.end(),
                   [&Attribute](const std::pair<unsigned, StorageRecord> &R) {
                     return R.second.Attribute == Attribute;
                   });
  if (Found == RecordMap.end()) {
    weak::DiagnosticError()
        << "Variable with attribute " << Attribute << " not found.";
    weak::UnreachablePoint();
  }
  return *Found;
}

[[noreturn]] static void EmitTypeError(TokenType Type) {
  weak::DiagnosticError()
      << "Type error: " << TokenToString(Type) << " expected.";
  weak::UnreachablePoint();
}

static void CheckIfVariableTypeIsSet(const Storage::StorageRecord &Record) {
  if (Record.DataType == TokenType::NONE) {
    weak::DiagnosticError()
        << "Internal error: type for " << Record.Name << " isn't set.";
    weak::UnreachablePoint();
  }
}

template <typename T>
static void SetVariableImpl(Storage::RecordMap &Records, TokenType Type,
                            unsigned Attribute, unsigned ScopeDepth,
                            T &&Value) {
  auto &[RecordAttribute, Record] = FindByAttribute(Records, Attribute);
  CheckIfVariableTypeIsSet(Record);

  if (Record.DataType != Type)
    EmitTypeError(Record.DataType);

  Record.Depth = ScopeDepth;
  Record.StoredValue = std::forward<T>(Value);
}

namespace weak {
namespace frontEnd {

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
  auto Found =
      std::find_if(Records.begin(), Records.end(),
                   [&Name](const std::pair<unsigned, StorageRecord> &R) {
                     return R.second.Name == Name;
                   });
  if (Found == Records.end()) {
    StorageRecord Record{/*Depth=*/CurrentScopeDepth,
                         /*Attribute=*/CurrentAttribute,
                         /*Name=*/Name.data(),
                         /*DataType=*/TokenType::NONE,
                         /*StoredValue=*/0};
    unsigned SavedAttribute = CurrentAttribute;
    Records.emplace(CurrentAttribute++, std::move(Record));
    return SavedAttribute;
  }

  const auto &[RecordAttribute, Record] = *Found;
  return RecordAttribute;
}

Storage::StorageRecord *Storage::GetSymbol(unsigned Attribute) {
  auto Found = Records.find(Attribute);

  if (Found == Records.end() || Found->second.Depth > CurrentScopeDepth) {
    DiagnosticError() << "Variable not found.";
    UnreachablePoint();
  }

  return &Found->second;
}

void Storage::SetSymbolType(unsigned Attribute, TokenType Type) {
  auto Found = Records.find(Attribute);
  if (Found == Records.end()) {
    DiagnosticError()
        << "Attempt to set type for variable that not exists.";
    UnreachablePoint();
  }
  Found->second.DataType = Type;
}

void Storage::SetIntValue(unsigned Attribute, signed Value) {
  SetVariableImpl(Records, TokenType::INTEGRAL_LITERAL, Attribute,
                  CurrentScopeDepth, Value);
}

void Storage::SetFloatValue(unsigned Attribute, float Value) {
  SetVariableImpl(Records, TokenType::FLOATING_POINT_LITERAL, Attribute,
                  CurrentScopeDepth, Value);
}

void Storage::SetCharValue(unsigned Attribute, char Value) {
  SetVariableImpl(Records, TokenType::CHAR, Attribute, CurrentScopeDepth,
                  Value);
}

void Storage::SetBoolValue(unsigned Attribute, bool Value) {
  SetVariableImpl(Records, TokenType::BOOLEAN, Attribute, CurrentScopeDepth,
                  Value);
}

void Storage::SetStringValue(unsigned Attribute, std::string Value) {
  SetVariableImpl(Records, TokenType::STRING_LITERAL, Attribute,
                  CurrentScopeDepth, std::move(Value));
}

} // namespace frontEnd
} // namespace weak