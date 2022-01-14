/* ASTSymbol.hpp - AST node to represent a variable name.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_FRONTEND_AST_AST_SYMBOL_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_SYMBOL_HPP

#include "FrontEnd/AST/ASTNode.hpp"
#include <string>

namespace weak {
namespace frontEnd {

class ASTSymbol : public ASTNode {
public:
  ASTSymbol(std::string TheValue, unsigned TheLineNo = 0U,
            unsigned TheColumnNo = 0U);

  ASTType GetASTType() const override;

  const std::string &GetValue() const;

private:
  std::string Value;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_SYMBOL_HPP
