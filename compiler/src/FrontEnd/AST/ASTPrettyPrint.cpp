#include "FrontEnd/AST/ASTPrettyPrint.hpp"
#include "FrontEnd/AST/ASTBinaryOperator.hpp"
#include "FrontEnd/AST/ASTBooleanLiteral.hpp"
#include "FrontEnd/AST/ASTFloatingPointLiteral.hpp"
#include "FrontEnd/AST/ASTIntegerLiteral.hpp"
#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/AST/ASTStringLiteral.hpp"
#include "FrontEnd/AST/ASTUnaryOperator.hpp"
#include <iostream>

using namespace weak::frontEnd;

namespace {
class IndentWidthHandler {
public:
  IndentWidthHandler(unsigned &TheWidth) : Width(TheWidth) { Width += 2; }
  ~IndentWidthHandler() { Width -= 2; }

  unsigned &Width;
};

} // namespace

static void ASTPrettyPrintImpl(const std::unique_ptr<ASTNode> &RootNode,
                               unsigned &IndentWidth);

static void PrintASTTextPosition(ASTNode *Node) {
  std::cout << " <line:" << Node->GetLineNo() << ", col:" << Node->GetColumnNo()
            << ">" << std::endl;
}

static void PrintBinaryASTNode(ASTNode *Node, unsigned &IndentWidth) {
  auto *Binary = static_cast<ASTBinaryOperator *>(Node);

  std::cout << "Binary " << TokenToString(Binary->GetOperation());
  PrintASTTextPosition(Binary);

  [[maybe_unused]] IndentWidthHandler _(IndentWidth);

  std::cout << std::string(IndentWidth, ' ');
  ASTPrettyPrintImpl(Binary->GetLHS(), IndentWidth);

  std::cout << std::string(IndentWidth, ' ');
  ASTPrettyPrintImpl(Binary->GetRHS(), IndentWidth);
}

static void PrintUnaryASTNode(ASTNode *Node, unsigned &IndentWidth) {
  auto *Unary = static_cast<ASTUnaryOperator *>(Node);

  std::cout << ((Unary->PrefixOrPostfix == ASTUnaryOperator::UnaryType::PREFIX)
                    ? "Prefix"
                    : "Postfix");
  std::cout << " unary " << TokenToString(Unary->GetOperation());
  PrintASTTextPosition(Unary);

  [[maybe_unused]] IndentWidthHandler _(IndentWidth);

  std::cout << std::string(IndentWidth, ' ');
  ASTPrettyPrintImpl(Unary->GetOperand(), IndentWidth);
}

template <typename DerivedNode>
static void PrintASTLiteral(std::string_view Label, ASTNode *Node) {
  auto *Derived = static_cast<DerivedNode *>(Node);
  std::cout << Label << Derived->GetValue() << std::endl;
}

static void PrintBooleanASTNode(ASTNode *Node) {
  PrintASTLiteral<ASTBooleanLiteral>("BooleanLiteral ", Node);
}

static void PrintFloatingPointASTNode(ASTNode *Node) {
  PrintASTLiteral<ASTFloatingPointLiteral>("FloatingPointLiteral ", Node);
}

static void PrintIntegerASTNode(ASTNode *Node) {
  PrintASTLiteral<ASTIntegerLiteral>("IntegerLiteral ", Node);
}

static void PrintStringASTNode(ASTNode *Node) {
  PrintASTLiteral<ASTStringLiteral>("StringLiteral ", Node);
}

void ASTPrettyPrintImpl(const std::unique_ptr<ASTNode> &RootNode,
                        unsigned &IndentWidth) {
  switch (RootNode->GetASTType()) {
  case ASTType::BINARY:
    PrintBinaryASTNode(RootNode.get(), IndentWidth);
    break;
  case ASTType::BOOLEAN_LITERAL:
    PrintBooleanASTNode(RootNode.get());
    break;
  case ASTType::FLOATING_POINT_LITERAL:
    PrintFloatingPointASTNode(RootNode.get());
    break;
  case ASTType::INTEGRAL_LITERAL:
    PrintIntegerASTNode(RootNode.get());
    break;
  case ASTType::STRING_LITERAL:
    PrintStringASTNode(RootNode.get());
    break;
  case ASTType::PREFIX_UNARY:
  case ASTType::POSTFIX_UNARY:
    PrintUnaryASTNode(RootNode.get(), IndentWidth);
    break;
  default:
    break;
  }
}

namespace weak {

void frontEnd::ASTPrettyPrint(const std::unique_ptr<ASTNode> &RootNode) {
  static unsigned IndentWidth = 0U;

  ASTPrettyPrintImpl(RootNode, IndentWidth);
  IndentWidth = 0;
}

} // namespace weak