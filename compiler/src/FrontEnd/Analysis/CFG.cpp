#include "FrontEnd/Analysis/CFG.hpp"

namespace weak {
namespace frontEnd {

CFG::CFG(CFGBlock *TheRootBlock) : RootBlock(TheRootBlock) {}

} // namespace frontEnd
} // namespace weak