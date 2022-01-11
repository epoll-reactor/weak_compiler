#include "FrontEnd/Symbols/Storage.hpp"
#include "Utility/Diagnostic.hpp"
#include <algorithm>

using namespace weak::frontEnd;

static const auto &FindByAttribute(const Storage::RecordMap &RecordMap,
                                   unsigned Attribute) {
  using StorageRecord = Storage::StorageRecord;
  auto Found =
      std::find_if(RecordMap.begin(), RecordMap.end(),
                   [&Attribute](const std::pair<unsigned, StorageRecord> &R) {
                     return R.second.Attribute == Attribute;
                   });
  if (Found == RecordMap.end()) {
    weak::DiagnosticError(0U, 0U)
        << "Variable with attribute " << Attribute << " not found.";
    weak::UnreachablePoint();
  }
  return *Found;
}

[[noreturn]] static void EmitTypeError(TokenType Type) {
  weak::DiagnosticError(0U, 0U)
      << "Type error: " << TokenToString(Type) << " expected.";
  weak::UnreachablePoint();
}

namespace weak {
namespace frontEnd {

Storage::Storage() : CurrentScopeDepth(0U), CurrentAttribute(0U), Records() {}

void Storage::ScopeBegin() { ++CurrentScopeDepth; }

void Storage::ScopeEnd() {
  if (CurrentScopeDepth == 0) {
    DiagnosticError(0U, 0U) << "No scopes left.";
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
    DiagnosticError(0U, 0U) << "Variable not found.";
    UnreachablePoint();
  }

  return &Found->second;
}

void Storage::SetSymbolType(unsigned Attribute, TokenType Type) {
  auto Found = Records.find(Attribute);
  if (Found == Records.end()) {
    DiagnosticError(0U, 0U)
        << "Attempt to set type for variable that not exists.";
    UnreachablePoint();
  }
  Found->second.DataType = Type;
}

void Storage::SetIntValue(unsigned Attribute, signed Value) {
  auto [RecordAttribute, Record] = FindByAttribute(Records, Attribute);

  if (Record.DataType != TokenType::INTEGRAL_LITERAL)
    EmitTypeError(Record.DataType);

  Record.StoredValue = Value;
}

void Storage::SetFloatValue(unsigned Attribute, float Value) {
  auto [RecordAttribute, Record] = FindByAttribute(Records, Attribute);

  if (Record.DataType != TokenType::FLOATING_POINT_LITERAL)
    EmitTypeError(Record.DataType);

  Record.StoredValue = Value;
}

void Storage::SetCharValue(unsigned Attribute, char Value) {
  auto [RecordAttribute, Record] = FindByAttribute(Records, Attribute);

  if (Record.DataType != TokenType::CHAR) //< Char isn't implemented yet...
    EmitTypeError(Record.DataType);

  Record.StoredValue = Value;
}

void Storage::SetBoolValue(unsigned Attribute, bool Value) {
  auto [RecordAttribute, Record] = FindByAttribute(Records, Attribute);

  if (Record.DataType != TokenType::BOOLEAN)
    EmitTypeError(Record.DataType);

  Record.StoredValue = Value;
}

void Storage::SetStringValue(unsigned Attribute, std::string Value) {
  auto [RecordAttribute, Record] = FindByAttribute(Records, Attribute);

  if (Record.DataType != TokenType::STRING_LITERAL)
    EmitTypeError(Record.DataType);

  Record.StoredValue = std::move(Value);
}

} // namespace frontEnd
} // namespace weak