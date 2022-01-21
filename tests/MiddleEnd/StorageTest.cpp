#include "MiddleEnd/Symbols/Storage.hpp"
#include "TestHelpers.hpp"
#include <cassert>

using namespace weak::frontEnd;
using namespace weak::middleEnd;

int main() {
  SECTION(BasicSymbols) {
    Storage Pool;

    Pool.ScopeBegin();

    unsigned Attribute1 = Pool.AddSymbol("var1");
    Pool.SetSymbolType(Attribute1, TokenType::STRING_LITERAL);

    Pool.ScopeBegin();

    unsigned Attribute2 = Pool.AddSymbol("var2");
    Pool.SetSymbolType(Attribute2, TokenType::BOOLEAN);

    unsigned Attribute3 = Pool.AddSymbol("var3");
    Pool.SetSymbolType(Attribute3, TokenType::FLOATING_POINT_LITERAL);

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


      Pool.SetSymbolType(Attribute2, TokenType::STRING_LITERAL);

      Pool.ScopeBegin();

        Pool.SetSymbolType(Attribute3, TokenType::INTEGRAL_LITERAL);

      assert(Pool.TotalVariables() == 3);
      Pool.ScopeEnd();
      assert(Pool.TotalVariables() == 2);

      Pool.SetSymbolType(Attribute2, TokenType::INTEGRAL_LITERAL);

    assert(Pool.TotalVariables() == 2);
    Pool.ScopeEnd();
    assert(Pool.TotalVariables() == 0);
    // clang-format on
  }
  SECTION(GetByName) {
    Storage Pool;

    Pool.ScopeBegin();

    unsigned Attribute1 = Pool.AddSymbol("var1");
    Pool.SetSymbolType(Attribute1, TokenType::STRING_LITERAL);

    Pool.ScopeEnd();
  }
}