#include "FrontEnd/AST/ASTVisitor.hpp"
#include "FrontEnd/AST/ASTBinaryOperator.hpp"
#include "FrontEnd/AST/ASTBooleanLiteral.hpp"
#include "FrontEnd/AST/ASTBreakStmt.hpp"
#include "FrontEnd/AST/ASTCompoundStmt.hpp"
#include "FrontEnd/AST/ASTContinueStmt.hpp"
#include "FrontEnd/AST/ASTDoWhileStmt.hpp"
#include "FrontEnd/AST/ASTFloatingPointLiteral.hpp"
#include "FrontEnd/AST/ASTForStmt.hpp"
#include "FrontEnd/AST/ASTFunctionDecl.hpp"
#include "FrontEnd/AST/ASTFunctionCall.hpp"
#include "FrontEnd/AST/ASTIfStmt.hpp"
#include "FrontEnd/AST/ASTIntegerLiteral.hpp"
#include "FrontEnd/AST/ASTNode.hpp"
#include "FrontEnd/AST/ASTPrettyPrint.hpp"
#include "FrontEnd/AST/ASTReturnStmt.hpp"
#include "FrontEnd/AST/ASTStringLiteral.hpp"
#include "FrontEnd/AST/ASTSymbol.hpp"
#include "FrontEnd/AST/ASTUnaryOperator.hpp"
#include "FrontEnd/AST/ASTVarDecl.hpp"
#include "FrontEnd/AST/ASTWhileStmt.hpp"

namespace weak {
namespace frontEnd {

void ASTVisitor::VisitBaseNode(const ASTNode *Node) const {
  switch (Node->GetASTType()) {
  case ASTType::BASE_NODE:
    break;
  case ASTType::INTEGER_LITERAL:
    Visit(static_cast<const ASTIntegerLiteral *>(Node));
    break;
  case ASTType::FLOATING_POINT_LITERAL:
    Visit(static_cast<const ASTFloatingPointLiteral *>(Node));
    break;
  case ASTType::STRING_LITERAL:
    Visit(static_cast<const ASTStringLiteral *>(Node));
    break;
  case ASTType::BOOLEAN_LITERAL:
    Visit(static_cast<const ASTBooleanLiteral *>(Node));
    break;
  case ASTType::SYMBOL:
    Visit(static_cast<const ASTSymbol *>(Node));
    break;
  case ASTType::VAR_DECL:
    Visit(static_cast<const ASTVarDecl *>(Node));
    break;
  case ASTType::PARAMETER:
    break;
  case ASTType::BREAK_STMT:
    Visit(static_cast<const ASTBreakStmt *>(Node));
    break;
  case ASTType::CONTINUE_STMT:
    Visit(static_cast<const ASTContinueStmt *>(Node));
    break;
  case ASTType::BINARY:
    Visit(static_cast<const ASTBinaryOperator *>(Node));
    break;
  case ASTType::PREFIX_UNARY: // Fall through.
  case ASTType::POSTFIX_UNARY:
    Visit(static_cast<const ASTUnaryOperator *>(Node));
    break;
  case ASTType::IF_STMT:
    Visit(static_cast<const ASTIfStmt *>(Node));
    break;
  case ASTType::FOR_STMT:
    Visit(static_cast<const ASTForStmt *>(Node));
    break;
  case ASTType::WHILE_STMT:
    Visit(static_cast<const ASTWhileStmt *>(Node));
    break;
  case ASTType::DO_WHILE_STMT:
    Visit(static_cast<const ASTDoWhileStmt *>(Node));
    break;
  case ASTType::RETURN_STMT:
    Visit(static_cast<const ASTReturnStmt *>(Node));
    break;
  case ASTType::COMPOUND_STMT:
    Visit(static_cast<const ASTCompoundStmt *>(Node));
    break;
  case ASTType::FUNCTION_DECL:
    Visit(static_cast<const ASTFunctionDecl *>(Node));
    break;
  case ASTType::FUNCTION_CALL:
    Visit(static_cast<const ASTFunctionCall *>(Node));
    break;
  }
}

} // namespace frontEnd
} // namespace weak