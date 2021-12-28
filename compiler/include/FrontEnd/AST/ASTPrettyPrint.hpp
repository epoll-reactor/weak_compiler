#ifndef WEAK_COMPILER_FRONTEND_AST_AST_PRETTY_PRINT_HPP
#define WEAK_COMPILER_FRONTEND_AST_AST_PRETTY_PRINT_HPP

#include "FrontEnd/AST/ASTNode.hpp"
#include <memory>

namespace weak {
namespace frontEnd {

/// Show visual representation of Syntax Tree beginning with
/// RootNode.
void ASTPrettyPrint(const std::unique_ptr<ASTNode> &RootNode);

} // namespace frontEnd
} // namespace weak

#endif // WEAK_COMPILER_FRONTEND_AST_AST_PRETTY_PRINT_HPP
