#include "FrontEnd/AST/ASTContinueStmt.hpp"

namespace weak {
namespace frontEnd {

ASTContinueStmt::ASTContinueStmt(unsigned TheLineNo, unsigned TheColumnNo)
    : ASTNode(TheLineNo, TheColumnNo) {}

ASTType ASTContinueStmt::GetASTType() const { return ASTType::CONTINUE_STMT; }

} // namespace frontEnd
} // namespace weak