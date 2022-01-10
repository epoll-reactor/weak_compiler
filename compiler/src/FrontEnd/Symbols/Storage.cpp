#include "FrontEnd/Symbols/Storage.hpp"
#include "Utility/Diagnostic.hpp"
#include <algorithm>

static auto FindByAttribute(const weak::frontEnd::Storage::RecordMap &RecordMap,
                            unsigned Attribute) {
  using namespace weak::frontEnd;
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
  return Found;
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
      DiagnosticWarning(0U, 0U) << "Cleaning " << It->second.Name;
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
    DiagnosticWarning(0U, 0U) << "Attribute: " << CurrentAttribute;
    StorageRecord Record{/*Depth=*/CurrentScopeDepth,
                         /*Attribute=*/CurrentAttribute,
                         /*Name=*/Name.data(),
                         /*DataType=*/TokenType::NONE,
                         /*StoredValue=*/0};
    unsigned SavedAttribute = CurrentAttribute;
    Records.emplace(CurrentAttribute++, std::move(Record));
    return SavedAttribute;
  }

  /// Suppress possible warning about unused Record variable.
  [[maybe_unused]] const auto &[RecordAttribute, Record] = *Found;
  return RecordAttribute;
}

Storage::StorageRecord *Storage::GetSymbol(unsigned Attribute) {
  auto Found = Records.find(Attribute);

  if (Found == Records.end() || Found->second.Depth > CurrentScopeDepth) {
    DiagnosticError(0U, 0U) << "Variable not found.";
  }

  return &Found->second;
}

void Storage::SetSymbolValue(unsigned Attribute, signed Value) {
  auto FoundIterator = *FindByAttribute(Records, Attribute);
  [[maybe_unused]] auto &[RecordAttribute, Record] = FoundIterator;

  //  if (Record.DataType != TokenType::INTEGRAL_LITERAL) {
  //    DiagnosticError(0U, 0U) << "Type error: integer expected.";
  //  }
  Record.StoredValue = Value;
}

void Storage::SetSymbolValue(unsigned Attribute, float Value) {
  auto FoundIterator = *FindByAttribute(Records, Attribute);
  [[maybe_unused]] auto &[RecordAttribute, Record] = FoundIterator;

  //  if (Record.DataType != TokenType::FLOATING_POINT_LITERAL) {
  //    DiagnosticError(0U, 0U) << "Type error: float expected.";
  //  }
  Record.StoredValue = Value;
}

void Storage::SetSymbolValue(unsigned Attribute, char Value) {
  auto FoundIterator = *FindByAttribute(Records, Attribute);
  [[maybe_unused]] auto &[RecordAttribute, Record] = FoundIterator;

  //  if (Record.DataType != TokenType::CHAR) { //< Char isn't implemented
  //  yet...
  //    DiagnosticError(0U, 0U) << "Type error: char expected.";
  //  }
  Record.StoredValue = Value;
}

void Storage::SetSymbolValue(unsigned Attribute, bool Value) {
  auto FoundIterator = *FindByAttribute(Records, Attribute);
  [[maybe_unused]] auto &[RecordAttribute, Record] = FoundIterator;

  //  if (TokenType Stored = Record.DataType;
  //      Stored != TokenType::TRUE && Stored != TokenType::FALSE) {
  //    DiagnosticError(0U, 0U) << "Type error: boolean expected.";
  //  }
  Record.StoredValue = Value;
}

void Storage::SetSymbolValue(unsigned Attribute, std::string Value) {
  auto FoundIterator = *FindByAttribute(Records, Attribute);
  [[maybe_unused]] auto &[RecordAttribute, Record] = FoundIterator;

  //  if (Record.DataType != TokenType::STRING_LITERAL) {
  //    DiagnosticError(0U, 0U) << "Type error: string literal expected.";
  //  }

  Record.StoredValue = Value;
}

} // namespace frontEnd
} // namespace weak