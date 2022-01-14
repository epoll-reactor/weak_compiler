/* ASTContinueStmt.cpp - AST node to represent a continue statement.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "FrontEnd/AST/ASTContinueStmt.hpp"

namespace weak {
namespace frontEnd {

ASTContinueStmt::ASTContinueStmt(unsigned TheLineNo, unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo) {}

ASTType ASTContinueStmt::GetASTType() const { return ASTType::CONTINUE_STMT; }

} // namespace frontEnd
} // namespace weak