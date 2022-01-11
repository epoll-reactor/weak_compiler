#include "FrontEnd/Symbols/Storage.hpp"
#include <cassert>

using namespace weak::frontEnd;

int main() {
  Storage VariableStorage;

  VariableStorage.ScopeBegin();

  unsigned Attribute1 = VariableStorage.AddSymbol("var1");
  VariableStorage.SetSymbolType(Attribute1, TokenType::STRING_LITERAL);
  VariableStorage.SetStringValue(Attribute1, "String value");

  VariableStorage.ScopeBegin();

  unsigned Attribute2 = VariableStorage.AddSymbol("var2");
  VariableStorage.SetSymbolType(Attribute2, TokenType::BOOLEAN);
  VariableStorage.SetBoolValue(Attribute2, true);

  unsigned Attribute3 = VariableStorage.AddSymbol("var3");
  VariableStorage.SetSymbolType(Attribute3, TokenType::FLOATING_POINT_LITERAL);
  VariableStorage.SetFloatValue(Attribute3, 1.23f);

  VariableStorage.ScopeEnd();

  VariableStorage.ScopeEnd();
}