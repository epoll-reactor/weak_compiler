#include "FrontEnd/AST/ASTPrettyPrint.hpp"
#include "FrontEnd/AST/ASTBinaryOperator.hpp"
#include "FrontEnd/AST/ASTBooleanLiteral.hpp"
#include "FrontEnd/AST/ASTBreakStmt.hpp"
#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include "FrontEnd/AST/ASTContinueStmt.hpp"
#include "FrontEnd/AST/ASTDoWhileStmt.hpp"
#include "FrontEnd/AST/ASTFloatingPointLiteral.hpp"
#include "FrontEnd/AST/ASTForStmt.hpp"
#include "FrontEnd/AST/ASTIfStmt.hpp"
#include "FrontEnd/AST/ASTIntegerLiteral.hpp"
#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/AST/ASTReturnStmt.hpp"
#include "FrontEnd/AST/ASTStringLiteral.hpp"
#include "FrontEnd/AST/ASTUnaryOperator.hpp"
#include "FrontEnd/AST/ASTVarDecl.hpp"
#include "FrontEnd/AST/ASTWhileStmt.hpp"
#include <iostream>

using namespace weak::frontEnd;

namespace {

class ASTNodePrinter {
public:
  ASTNodePrinter(ASTNode *TheRoot) : Root(TheRoot), NestedIndentWidth(0U) {}

  void Print() { PrintNode(Root); }

private:
  void PrintNode(ASTNode *Node) {
    switch (Node->GetASTType()) {
    case ASTType::BASE_NODE:
      break;
    case ASTType::INTEGER_LITERAL:
      PrintNode(static_cast<ASTIntegerLiteral *>(Node));
      break;
    case ASTType::FLOATING_POINT_LITERAL:
      PrintNode(static_cast<ASTFloatingPointLiteral *>(Node));
      break;
    case ASTType::STRING_LITERAL:
      PrintNode(static_cast<ASTStringLiteral *>(Node));
      break;
    case ASTType::BOOLEAN_LITERAL:
      PrintNode(static_cast<ASTBooleanLiteral *>(Node));
      break;
    case ASTType::PARAMETER: // Not implemented yet.
      break;
    case ASTType::BREAK_STMT:
      PrintNode(static_cast<ASTBreakStmt *>(Node));
      break;
    case ASTType::CONTINUE_STMT:
      PrintNode(static_cast<ASTContinueStmt *>(Node));
      break;
    case ASTType::BINARY:
      PrintNode(static_cast<ASTBinaryOperator *>(Node));
      break;
    case ASTType::PREFIX_UNARY: // Fall through.
    case ASTType::POSTFIX_UNARY:
      PrintNode(static_cast<ASTUnaryOperator *>(Node));
      break;
    case ASTType::IF_STMT:
      PrintNode(static_cast<ASTIfStmt *>(Node));
      break;
    case ASTType::FOR_STMT:
      PrintNode(static_cast<ASTForStmt *>(Node));
      break;
    case ASTType::WHILE_STMT:
      PrintNode(static_cast<ASTWhileStmt *>(Node));
      break;
    case ASTType::DO_WHILE_STMT:
      PrintNode(static_cast<ASTDoWhileStmt *>(Node));
      break;
    case ASTType::RETURN_STMT:
      PrintNode(static_cast<ASTReturnStmt *>(Node));
      break;
    case ASTType::COMPOUND_STMT:
      PrintNode(static_cast<ASTCompoundStmt *>(Node));
      break;
    case ASTType::VAR_DECL:
      PrintNode(static_cast<ASTVarDecl *>(Node));
    default:
      break;
    }
  }

  void PrintNode(ASTBinaryOperator *Binary) {
    using namespace std::string_literals;
    PrintLabelWithTextPosition(
        "BinaryOperator "s + TokenToString(Binary->GetOperation()), Binary);
    [[maybe_unused]] IndentPrinter _(NestedIndentWidth);

    PrintIndent();
    PrintNode(Binary->GetLHS().get());

    PrintIndent();
    PrintNode(Binary->GetRHS().get());
  }

  void PrintNode(ASTUnaryOperator *Unary) {
    using UnaryType = ASTUnaryOperator::UnaryType;
    std::cout << ((Unary->PrefixOrPostfix == UnaryType::PREFIX) ? "Prefix"
                                                                : "Postfix");
    using namespace std::string_literals;
    PrintLabelWithTextPosition(
        "UnaryOperator "s + TokenToString(Unary->GetOperation()), Unary);
    [[maybe_unused]] IndentPrinter _(NestedIndentWidth);

    PrintIndent();
    PrintNode(Unary->GetOperand().get());
  }

  void PrintNode(ASTIfStmt *If) {
    PrintLabelWithTextPosition("IfStmt", If);
    [[maybe_unused]] IndentPrinter ConditionLabelIndent(NestedIndentWidth);
    PrintIndent();

    PrintLabelWithTextPosition("IfCondition", If);

    if (const auto &Condition = If->GetCondition()) {
      [[maybe_unused]] IndentPrinter _(NestedIndentWidth);
      PrintIndent();
      PrintNode(Condition.get());
    }

    if (const auto &ThenBody = If->GetThenBody())
      PrintCompoundStatement("IfThenBody", ThenBody.get());

    if (const auto &ElseBody = If->GetElseBody())
      PrintCompoundStatement("IfElseBody", ElseBody.get());
  }

  void PrintLoopControlStmt(std::string_view Label, ASTNode *Node) {
    PrintIndent();
    PrintLabelWithTextPosition(Label, Node);
    [[maybe_unused]] IndentPrinter _(NestedIndentWidth);
    PrintIndent();
    PrintNode(Node);
  }

  void PrintNode(ASTForStmt *For) {
    PrintLabelWithTextPosition("ForStmt", For);

    [[maybe_unused]] IndentPrinter ForStmtIndent(NestedIndentWidth);

    if (const auto &Init = For->GetInit())
      PrintLoopControlStmt("ForStmtInit", Init.get());

    if (const auto &Condition = For->GetCondition())
      PrintLoopControlStmt("ForStmtCondition", Condition.get());

    if (const auto &Increment = For->GetIncrement())
      PrintLoopControlStmt("ForStmtIncrement", Increment.get());

    if (const auto &ForBody = For->GetBody())
      PrintCompoundStatement("ForStmtBody", ForBody.get());
  }

  template <typename DerivedNode>
  void PrintCommonWhileStmt(DerivedNode *CommonWhile, bool IsDoWhile) {
    using namespace std::string_literals;
    PrintLabelWithTextPosition((IsDoWhile ? "Do"s : ""s) + "WhileStmt",
                               CommonWhile);

    [[maybe_unused]] IndentPrinter WhileStmtIndent(NestedIndentWidth);

    auto PrintWhileCondition = [&, this] {
      if (const auto &Condition = CommonWhile->GetCondition())
        PrintLoopControlStmt((IsDoWhile ? "Do"s : ""s) + "WhileStmtCond",
                             Condition.get());
    };

    auto PrintWhileBody = [&, this] {
      if (const auto &Body = CommonWhile->GetBody())
        PrintCompoundStatement((IsDoWhile ? "Do"s : ""s) + "WhileStmtBody",
                               Body.get());
    };

    if (IsDoWhile) {
      PrintWhileBody();
      PrintWhileCondition();
    } else {
      PrintWhileCondition();
      PrintWhileBody();
    }
  }

  void PrintNode(ASTWhileStmt *While) {
    PrintCommonWhileStmt(While, /*IsDoWhile=*/false);
  }

  void PrintNode(ASTDoWhileStmt *While) {
    PrintCommonWhileStmt(While, /*IsDoWhile=*/true);
  }

  void PrintNode(ASTCompoundStmt *Compound) {
    PrintIndent();
    PrintLabelWithTextPosition("CompoundStmt", Compound);

    [[maybe_unused]] IndentPrinter _(NestedIndentWidth);
    for (const auto &Stmt : Compound->GetStmts()) {
      PrintIndent();
      PrintNode(Stmt.get());
    }
  }

  void PrintNode(ASTReturnStmt *Return) {
    PrintIndent();
    PrintLabelWithTextPosition("ReturnStmt", Return);

    [[maybe_unused]] IndentPrinter _(NestedIndentWidth);
    PrintIndent();
    PrintNode(Return->GetOperand().get());
  }

  void PrintNode(ASTVarDecl *Decl) {
    PrintIndent();
    PrintLabelWithTextPosition("VarDeclStmt", Decl, /*PrintNewLine=*/false);
    std::cout << " " << TokenToString(Decl->GetDataType());
    std::cout << " " << Decl->GetSymbolName() << std::endl;

    [[maybe_unused]] IndentPrinter _(NestedIndentWidth);
    PrintIndent();

    PrintNode(Decl->GetDeclareBody().get());
  }

  template <typename DerivedNode>
  void PrintLiteral(std::string_view Label, ASTNode *Node) {
    PrintLabelWithTextPosition(Label, Node, /*PrintNewLine=*/false);
    std::cout << " ";
    std::cout << static_cast<DerivedNode *>(Node)->GetValue() << std::endl;
  }

  void PrintNode(ASTBooleanLiteral *Boolean) {
    PrintLiteral<ASTBooleanLiteral>("BooleanLiteral", Boolean);
  }

  void PrintNode(ASTFloatingPointLiteral *Float) {
    PrintLiteral<ASTFloatingPointLiteral>("FloatLiteral", Float);
  }

  void PrintNode(ASTIntegerLiteral *Integer) {
    PrintLiteral<ASTIntegerLiteral>("IntLiteral", Integer);
  }

  void PrintNode(ASTStringLiteral *Float) {
    PrintLiteral<ASTStringLiteral>("StringLiteral", Float);
  }

  void PrintNode(ASTBreakStmt *Break) {
    PrintLabelWithTextPosition("BreakStmt", Break);
  }

  void PrintNode(ASTContinueStmt *Continue) {
    PrintLabelWithTextPosition("ContinueStmt", Continue);
  }

  void PrintLabelWithTextPosition(std::string_view Label, ASTNode *Node,
                                  bool PrintNewLine = true) {
    std::cout << Label << " <line:" << Node->GetLineNo()
              << ", col:" << Node->GetColumnNo() << ">";
    if (PrintNewLine)
      std::cout << std::endl;
  }

  void PrintIndent() { std::cout << std::string(NestedIndentWidth, ' '); }

  void PrintCompoundStatement(std::string_view Label,
                              ASTCompoundStmt *CompoundStmt) {
    PrintIndent();
    PrintLabelWithTextPosition(Label, CompoundStmt);
    [[maybe_unused]] IndentPrinter _indent_2(NestedIndentWidth);
    PrintNode(CompoundStmt);
  }

  struct IndentPrinter {
    IndentPrinter(unsigned &TheWidth) : Width(TheWidth) { Width += IndentSize; }
    ~IndentPrinter() { Width -= IndentSize; }

    unsigned &Width;
    static constexpr unsigned IndentSize = 2U;
  };

  ASTNode *Root;
  unsigned NestedIndentWidth;
};

} // namespace

namespace weak {

void frontEnd::ASTPrettyPrint(const std::unique_ptr<ASTNode> &RootNode) {
  ASTNodePrinter Printer(RootNode.get());
  Printer.Print();
}

} // namespace weak