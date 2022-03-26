/* IRNodePrinter.cpp - Simplified AST dump for IR statements.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#include "MiddleEnd/IR/IRNodePrinter.hpp"
#include "FrontEnd/AST/ASTBinaryOperator.hpp"
#include "FrontEnd/AST/ASTBooleanLiteral.hpp"
#include "FrontEnd/AST/ASTBreakStmt.hpp"
#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include "FrontEnd/AST/ASTContinueStmt.hpp"
#include "FrontEnd/AST/ASTDoWhileStmt.hpp"
#include "FrontEnd/AST/ASTFloatingPointLiteral.hpp"
#include "FrontEnd/AST/ASTForStmt.hpp"
#include "FrontEnd/AST/ASTFunctionCall.hpp"
#include "FrontEnd/AST/ASTFunctionDecl.hpp"
#include "FrontEnd/AST/ASTIfStmt.hpp"
#include "FrontEnd/AST/ASTIntegerLiteral.hpp"
#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/AST/ASTReturnStmt.hpp"
#include "FrontEnd/AST/ASTStringLiteral.hpp"
#include "FrontEnd/AST/ASTSymbol.hpp"
#include "FrontEnd/AST/ASTUnaryOperator.hpp"
#include "FrontEnd/AST/ASTVarDecl.hpp"
#include "FrontEnd/AST/ASTWhileStmt.hpp"
#include <sstream>

using namespace weak::frontEnd;

namespace weak {
namespace middleEnd {

IRNodePrinter::IRNodePrinter(frontEnd::ASTNode *TheRootNode,
                             std::ostream &TheOutStream)
    : RootNode(TheRootNode), OutStream(TheOutStream) {}

void IRNodePrinter::Print() { RootNode->Accept(this); }

void IRNodePrinter::Visit(const ASTBinaryOperator *Stmt) const {
  Stmt->GetLHS()->Accept(this);
  OutStream << TokenToString(Stmt->GetOperation());
  Stmt->GetRHS()->Accept(this);
}

void IRNodePrinter::Visit(const ASTBooleanLiteral *Stmt) const {
  OutStream << std::boolalpha << Stmt->GetValue();
}

void IRNodePrinter::Visit(const ASTFloatingPointLiteral *Stmt) const {
  OutStream << Stmt->GetValue();
}

void IRNodePrinter::Visit(const ASTIntegerLiteral *Stmt) const {
  OutStream << Stmt->GetValue();
}

void IRNodePrinter::Visit(const ASTStringLiteral *Stmt) const {
  OutStream << "\"" << Stmt->GetValue() << "\"";
}

void IRNodePrinter::Visit(const ASTSymbol *Stmt) const {
  OutStream << Stmt->GetSSAName();
}

void IRNodePrinter::Visit(const ASTUnaryOperator *Stmt) const {
  OutStream << (Stmt->PrefixOrPostfix == ASTUnaryOperator::UnaryType::PREFIX
                    ? "Prefix "
                    : "Postfix ");
  Stmt->GetOperand()->Accept(this);
  OutStream << TokenToString(Stmt->GetOperation());
}

void IRNodePrinter::Visit(const ASTBreakStmt *) const {
  OutStream << "<not implemented>";
}

void IRNodePrinter::Visit(const ASTCompoundStmt *) const {
  OutStream << "<not implemented>";
}

void IRNodePrinter::Visit(const ASTContinueStmt *) const {
  OutStream << "<not implemented>";
}

void IRNodePrinter::Visit(const ASTDoWhileStmt *) const {
  OutStream << "<not implemented>";
}

void IRNodePrinter::Visit(const ASTForStmt *) const {
  OutStream << "<not implemented>";
}

void IRNodePrinter::Visit(const ASTFunctionDecl *) const {
  OutStream << "<not implemented>";
}

void IRNodePrinter::Visit(const ASTFunctionCall *) const {
  OutStream << "<not implemented>";
}

void IRNodePrinter::Visit(const ASTIfStmt *) const {
  OutStream << "<not implemented>";
}

void IRNodePrinter::Visit(const ASTReturnStmt *) const {
  OutStream << "<not implemented>";
}

void IRNodePrinter::Visit(const ASTVarDecl *) const {
  OutStream << "<not implemented>";
}

void IRNodePrinter::Visit(const ASTWhileStmt *) const {
  OutStream << "<not implemented>";
}

} // namespace middleEnd
} // namespace weak