/* TargetCodeBuilder.hpp - Builder of executable code from LLVM IR.
 * Copyright (C) 2022 epoll-reactor <glibcxx.chrono@gmail.com>
 *
 * This file is distributed under the MIT license.
 */

#ifndef WEAK_COMPILER_MIDDLE_END_TARGET_CODE_BUILDER_HPP
#define WEAK_COMPILER_MIDDLE_END_TARGET_CODE_BUILDER_HPP

#include "llvm/IR/Module.h"

namespace weak {
namespace middleEnd {

class TargetCodeBuilder {
public:
  TargetCodeBuilder(llvm::Module &TheModule,
                    std::string_view TheObjectFilePath);

  void Build();

private:
  llvm::Module &Module;
  std::string ObjectFilePath;
};

} // namespace middleEnd
} // namespace weak

#endif // WEAK_COMPILER_MIDDLE_END_TARGET_CODE_BUILDER_HPP
