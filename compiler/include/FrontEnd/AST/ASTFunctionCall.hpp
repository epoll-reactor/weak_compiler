/* ASTFunctionCall.hpp - AST node to represent a function call statement.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_FRONTEND_AST_AST_FUNCTION_CALL_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_FUNCTION_CALL_HPP

#include "FrontEnd/AST/ASTNode.hpp"
#include <string>
#include <vector>

namespace weak {
namespace frontEnd {

class ASTFunctionCall : public ASTNode {
public:
  ASTFunctionCall(std::string &&TheName,
                  std::vector<std::unique_ptr<ASTNode>> &&TheArguments,
                  unsigned TheLineNo = 0U, unsigned TheColumnNo = 0U);

  ASTType GetASTType() const override;
  void Accept(const ASTVisitor *) const override;

  const std::string &GetName() const;
  const std::vector<std::unique_ptr<ASTNode>> &GetArguments() const;

private:
  std::string Name;
  std::vector<std::unique_ptr<ASTNode>> Arguments;
};

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_FUNCTION_CALL_HPP
