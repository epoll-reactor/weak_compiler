#include "FrontEnd/AST/ASTPrettyPrint.hpp"
#include "FrontEnd/AST/ASTBinaryOperator.hpp"
#include "FrontEnd/AST/ASTBooleanLiteral.hpp"
#include "FrontEnd/AST/ASTBreakStmt.hpp"
#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include "FrontEnd/AST/ASTContinueStmt.hpp"
#include "FrontEnd/AST/ASTDoWhileStmt.hpp"
#include "FrontEnd/AST/ASTFloatingPointLiteral.hpp"
#include "FrontEnd/AST/ASTForStmt.hpp"
#include "FrontEnd/AST/ASTFunctionDecl.hpp"
#include "FrontEnd/AST/ASTIfStmt.hpp"
#include "FrontEnd/AST/ASTIntegerLiteral.hpp"
#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/AST/ASTReturnStmt.hpp"
#include "FrontEnd/AST/ASTStringLiteral.hpp"
#include "FrontEnd/AST/ASTSymbol.hpp"
#include "FrontEnd/AST/ASTUnaryOperator.hpp"
#include "FrontEnd/AST/ASTVarDecl.hpp"
#include "FrontEnd/AST/ASTVisitor.hpp"
#include "FrontEnd/AST/ASTWhileStmt.hpp"
#include <iostream>

using namespace weak::frontEnd;

namespace {

class ASTPrintVisitor : public ASTVisitor {
public:
  ASTPrintVisitor(ASTNode *TheRootNode) : RootNode(TheRootNode), Indent(0U) {}

  void Print() const { VisitBaseNode(RootNode); }

private:
  void Visit(const ASTBinaryOperator *Binary) const override {
    PrintWithTextPosition("BinaryOperator", Binary, /*NewLineNeeded=*/false);
    std::cout << TokenToString(Binary->GetOperation()) << std::endl;
    Indent += 2;

    PrintIndent();
    VisitBaseNode(Binary->GetLHS().get());

    PrintIndent();
    VisitBaseNode(Binary->GetRHS().get());

    Indent -= 2;
  }

  void Visit(const ASTBooleanLiteral *Boolean) const override {
    PrintWithTextPosition("BooleanLiteral", Boolean, /*NewLineNeeded=*/false);
    std::cout << std::boolalpha << Boolean->GetValue() << std::endl;
  }

  void Visit(const ASTBreakStmt *BreakStmt) const override {
    PrintWithTextPosition("BreakStmt", BreakStmt, /*NewLineNeeded=*/true);
  }

  void Visit(const ASTCompoundStmt *CompoundStmt) const override {
    PrintWithTextPosition("CompoundStmt", CompoundStmt, /*NewLineNeeded=*/true);

    Indent += 2;
    for (const auto &Stmt : CompoundStmt->GetStmts()) {
      PrintIndent();
      VisitBaseNode(Stmt.get());
    }
    Indent -= 2;
  }

  void Visit(const ASTContinueStmt *ContinueStmt) const override {
    PrintWithTextPosition("ContinueStmt", ContinueStmt, /*NewLineNeeded=*/true);
  }

  void Visit(const ASTFloatingPointLiteral *Float) const override {
    PrintWithTextPosition("FloatingPointLiteral", Float,
                          /*NewLineNeeded=*/false);
    std::cout << Float->GetValue() << std::endl;
  }

  void Visit(const ASTForStmt *ForStmt) const override {
    PrintWithTextPosition("ForStmt", ForStmt,
                          /*NewLineNeeded=*/true);

    Indent += 2;

    if (const auto *Init = ForStmt->GetInit().get()) {
      PrintIndent();
      PrintWithTextPosition("ForStmtInit", Init,
                            /*NewLineNeeded=*/true);
      Indent += 2;
      PrintIndent();
      VisitBaseNode(Init);
      Indent -= 2;
    }

    if (const auto *Condition = ForStmt->GetCondition().get()) {
      PrintIndent();
      PrintWithTextPosition("ForStmtCondition", Condition,
                            /*NewLineNeeded=*/true);
      Indent += 2;
      PrintIndent();
      VisitBaseNode(Condition);
      Indent -= 2;
    }

    if (const auto *Increment = ForStmt->GetIncrement().get()) {
      PrintIndent();
      PrintWithTextPosition("ForStmtIncrement", Increment,
                            /*NewLineNeeded=*/true);
      Indent += 2;
      PrintIndent();
      VisitBaseNode(Increment);
      Indent -= 2;
    }

    if (const auto *Body = ForStmt->GetBody().get()) {
      PrintIndent();
      PrintWithTextPosition("ForStmtBody", Body,
                            /*NewLineNeeded=*/true);
      Indent += 2;
      PrintIndent();
      Visit(Body);
      Indent -= 2;
    }

    Indent -= 2;
  }

  void Visit(const ASTIfStmt *IfStmt) const override {
    PrintWithTextPosition("IfStmt", IfStmt, /*NewLineNeeded=*/true);
    Indent += 2;

    if (const auto &Condition = IfStmt->GetCondition()) {
      PrintIndent();
      PrintWithTextPosition("IfStmtCondition", Condition.get(),
                            /*NewLineNeeded=*/true);
      Indent += 2;
      PrintIndent();
      VisitBaseNode(Condition.get());
      Indent -= 2;
    }

    if (const auto &ThenBody = IfStmt->GetThenBody()) {
      PrintIndent();
      PrintWithTextPosition("IfStmtThenBody", ThenBody.get(),
                            /*NewLineNeeded=*/true);
      Indent += 2;
      PrintIndent();
      Visit(ThenBody.get());
      Indent -= 2;
    }

    if (const auto &ElseBody = IfStmt->GetElseBody()) {
      PrintIndent();
      PrintWithTextPosition("IfStmtElseBody", ElseBody.get(),
                            /*NewLineNeeded=*/true);
      Indent += 2;
      PrintIndent();
      Visit(ElseBody.get());
      Indent -= 2;
    }
    Indent -= 2;
  }

  void Visit(const ASTIntegerLiteral *Integer) const override {
    PrintWithTextPosition("IntegerLiteral", Integer,
                          /*NewLineNeeded*/ false);
    std::cout << Integer->GetValue() << std::endl;
  }

  void Visit(const ASTReturnStmt *ReturnStmt) const override {
    PrintWithTextPosition("ReturnStmt", ReturnStmt, /*NewLineNeeded=*/true);
    Indent += 2;

    if (ReturnStmt->GetOperand()) {
      PrintIndent();
      VisitBaseNode(ReturnStmt->GetOperand().get());
    }

    Indent -= 2;
  }

  void Visit(const ASTStringLiteral *String) const override {
    PrintWithTextPosition("StringLiteral", String,
                          /*NewLineNeeded=*/false);
    std::cout << String->GetValue() << std::endl;
  }

  void Visit(const ASTSymbol *Symbol) const override {
    PrintWithTextPosition("Symbol", Symbol,
                          /*NewLineNeeded=*/false);
    std::cout << Symbol->GetValue() << std::endl;
  }

  void Visit(const ASTUnaryOperator *Unary) const override {
    std::cout << (Unary->PrefixOrPostfix == ASTUnaryOperator::UnaryType::PREFIX
                      ? "Prefix "
                      : "Postfix ");
    PrintWithTextPosition("UnaryOperator", Unary,
                          /*NewLineNeeded=*/false);
    std::cout << TokenToString(Unary->GetOperation()) << std::endl;
    Indent += 2;

    PrintIndent();
    VisitBaseNode(Unary->GetOperand().get());

    Indent -= 2;
  }

  void Visit(const ASTVarDecl *VarDecl) const override {
    PrintWithTextPosition("VarDeclStmt", VarDecl, /*NewLineNeeded=*/false);
    std::cout << TokenToString(VarDecl->GetDataType()) << " "
              << VarDecl->GetSymbolName() << std::endl;

    if (const auto &Body = VarDecl->GetDeclareBody()) {
      Indent += 2;
      PrintIndent();
      VisitBaseNode(Body.get());
      Indent -= 2;
    }
  }

  void Visit(const ASTFunctionDecl *FunctionDecl) const override {
    PrintWithTextPosition("FunctionDecl", FunctionDecl, /*NewLineNeeded=*/true);

    Indent += 2;
    PrintIndent();
    PrintWithTextPosition("FunctionRetType", FunctionDecl,
                          /*NewLineNeeded=*/false);
    std::cout << TokenToString(FunctionDecl->GetReturnType()) << std::endl;

    PrintIndent();
    PrintWithTextPosition("FunctionName", FunctionDecl,
                          /*NewLineNeeded=*/false);
    std::cout << FunctionDecl->GetName() << std::endl;

    PrintIndent();
    PrintWithTextPosition("FunctionArgs", FunctionDecl,
                          /*NewLineNeeded=*/true);

    Indent += 2;
    for (const auto &Argument : FunctionDecl->GetArguments()) {
      PrintIndent();
      VisitBaseNode(Argument.get());
    }
    Indent -= 2;

    PrintIndent();
    PrintWithTextPosition("FunctionBody", FunctionDecl,
                          /*NewLineNeeded=*/true);

    Indent += 2;
    PrintIndent();
    Visit(FunctionDecl->GetBody().get());
    Indent -= 2;
  }

  void Visit(const ASTDoWhileStmt *DoWhileStmt) const override {
    CommonWhileStmtVisit(DoWhileStmt, /*IsDoWhile=*/true);
  }

  void Visit(const ASTWhileStmt *WhileStmt) const override {
    CommonWhileStmtVisit(WhileStmt, /*IsDoWhile=*/false);
  }

  template <typename WhileNode>
  void CommonWhileStmtVisit(const WhileNode *WhileStmt, bool IsDoWhile) const {
    using namespace std::string_literals;
    PrintWithTextPosition((IsDoWhile ? "Do"s : ""s) + "WhileStmt", WhileStmt,
                          /*NewLineNeeded=*/true);

    auto PrintWhileCondition = [&, this] {
      if (const auto &Condition = WhileStmt->GetCondition().get()) {
        PrintIndent();
        PrintWithTextPosition((IsDoWhile ? "Do"s : ""s) + "WhileStmtCond",
                              Condition,
                              /*NewLineNeeded=*/true);
        Indent += 2;
        PrintIndent();
        VisitBaseNode(Condition);
        Indent -= 2;
      }
    };

    auto PrintWhileBody = [&, this] {
      if (const auto &Body = WhileStmt->GetBody().get()) {
        PrintIndent();
        PrintWithTextPosition((IsDoWhile ? "Do"s : ""s) + "WhileStmtBody", Body,
                              /*NewLineNeeded=*/true);
        Indent += 2;
        PrintIndent();
        Visit(Body);
        Indent -= 2;
      }
    };

    Indent += 2;

    if (IsDoWhile) {
      PrintWhileBody();
      PrintWhileCondition();
    } else {
      PrintWhileCondition();
      PrintWhileBody();
    }

    Indent -= 2;
  }

  void PrintWithTextPosition(std::string_view Label, const ASTNode *Node,
                             bool NewLineNeeded) const {
    std::cout << Label << " <line:" << Node->GetLineNo()
              << ", col:" << Node->GetColumnNo() << ">";

    if (NewLineNeeded)
      std::cout << std::endl;
    else
      std::cout << " ";
  }

  void PrintIndent() const { std::cout << std::string(Indent, ' '); }

  ASTNode *RootNode;
  mutable unsigned Indent;
};

} // namespace

namespace weak {

void frontEnd::ASTPrettyPrint(const std::unique_ptr<ASTNode> &RootNode) {
  ASTPrintVisitor Printer(RootNode.get());
  Printer.Print();
}

} // namespace weak