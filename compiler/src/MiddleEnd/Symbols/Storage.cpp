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

void Storage::ScopeBegin() { ++CurrentScopeDepth; }

void Storage::ScopeEnd() {
  if (CurrentScopeDepth == 0) {
    CompileError() << "No scopes left.";
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
                            [&](const std::pair<unsigned, Record> &R) {
                              return R.second.Name == Name;
                            });
  if (Found == Records.end()) {
    auto CreateBuzzReference = [] {
      return Reference(UnaryInstruction(0U, 0));
    };
    Record Variable{/*Depth=*/CurrentScopeDepth,
                    /*Attribute=*/CurrentAttribute,
                    /*TemporaryLabel=*/0U,
                    /*Name=*/Name.data(),
                    /*DataType=*/TokenType::NONE,
                    /*Reference=*/CreateBuzzReference()};
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
    CompileError() << "Variable not found.";
    UnreachablePoint();
  }

  return &Found->second;
}

Storage::Record *Storage::GetByName(std::string_view Name) {
  auto Found = std::find_if(Records.begin(), Records.end(),
                            [&](const std::pair<unsigned, Record> &R) {
                              return R.second.Name == Name;
                            });
  if (Found == Records.end()) {
    CompileError() << "Variable not found: " << Name;
  }

  auto &[_, Variable] = *Found;
  return &Variable;
}

void Storage::SetSymbolType(unsigned Attribute, TokenType Type) {
  auto Found = Records.find(Attribute);
  if (Found == Records.end()) {
    CompileError() << "Attempt to set type for variable that not exists.";
    UnreachablePoint();
  }
  Found->second.DataType = Type;
}

} // namespace middleEnd
} // namespace weak