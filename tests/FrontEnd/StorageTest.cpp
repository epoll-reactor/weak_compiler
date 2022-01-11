#include "FrontEnd/Symbols/Storage.hpp"
#include "../TestHelpers.hpp"
#include <cassert>

using namespace weak::frontEnd;

int main() {
  SECTION(BasicSymbols) {
    Storage Pool;

    Pool.ScopeBegin();

    unsigned Attribute1 = Pool.AddSymbol("var1");
    Pool.SetSymbolType(Attribute1, TokenType::STRING_LITERAL);
    Pool.SetStringValue(Attribute1, "String value");

    Pool.ScopeBegin();

    unsigned Attribute2 = Pool.AddSymbol("var2");
    Pool.SetSymbolType(Attribute2, TokenType::BOOLEAN);
    Pool.SetBoolValue(Attribute2, true);

    unsigned Attribute3 = Pool.AddSymbol("var3");
    Pool.SetSymbolType(Attribute3,
                                  TokenType::FLOATING_POINT_LITERAL);
    Pool.SetFloatValue(Attribute3, 1.23f);

    Pool.ScopeEnd();

    Pool.ScopeEnd();
  }
  SECTION(Scopes) {
    Storage Pool;

    /// This is done by lexical analyzer.
    unsigned Attribute1 = Pool.AddSymbol("var1");
    unsigned Attribute2 = Pool.AddSymbol("var2");
    unsigned Attribute3 = Pool.AddSymbol("var3");

    /// This is done by static analyzer/code generator.
    // clang-format off
    Pool.ScopeBegin();

      Pool.SetSymbolType(Attribute1, TokenType::INTEGRAL_LITERAL);
      Pool.SetIntValue(Attribute1, 1);

      const Storage::AnyDataType &Value1 = Pool.GetSymbol(Attribute1)->StoredValue;
      assert(std::get<signed>(Value1) == 1);

      Pool.SetSymbolType(Attribute2, TokenType::STRING_LITERAL);
      Pool.SetStringValue(Attribute2, "Variable two");

      const Storage::AnyDataType &Value2 = Pool.GetSymbol(Attribute2)->StoredValue;
      assert(std::get<std::string>(Value2) == "Variable two");

      Pool.ScopeBegin();

        Pool.SetSymbolType(Attribute3, TokenType::INTEGRAL_LITERAL);
        Pool.SetIntValue(Attribute3, 3);

        const Storage::AnyDataType &Value3 = Pool.GetSymbol(Attribute3)->StoredValue;
        assert(std::get<signed>(Value3) == 3);

        Pool.SetIntValue(Attribute3, 4);
        assert(std::get<signed>(Value3) == 4);

      assert(Pool.TotalVariables() == 3);
      Pool.ScopeEnd();
      assert(Pool.TotalVariables() == 2);

      Pool.SetSymbolType(Attribute2, TokenType::INTEGRAL_LITERAL);
      Pool.SetIntValue(Attribute2, 3);

    assert(Pool.TotalVariables() == 2);
    Pool.ScopeEnd();
    assert(Pool.TotalVariables() == 0);
    // clang-format on
  }
}