#include "FrontEnd/Symbols/Storage.hpp"
#include <cassert>

using namespace weak::frontEnd;

int main() {
  Storage VariableStorage;

  VariableStorage.ScopeBegin();

  unsigned Attribute1 = VariableStorage.AddSymbol("var1");
  VariableStorage.SetSymbolValue(Attribute1, "String value");

  VariableStorage.ScopeBegin();

  unsigned Attribute2 = VariableStorage.AddSymbol("var2");
  VariableStorage.SetSymbolValue(Attribute2, true);

  unsigned Attribute3 = VariableStorage.AddSymbol("var3");
  VariableStorage.SetSymbolValue(Attribute3, true);

  VariableStorage.ScopeEnd();

  VariableStorage.ScopeEnd();
}