#ifndef COMPILER_TEST_HELPERS_HPP
#define COMPILER_TEST_HELPERS_HPP

#include <iostream>

#define TEST_CASE(stmt)                                                        \
  if (!(stmt)) {                                                               \
    std::cerr << "ERROR: test case " << #stmt << " failed\n";                  \
    exit(1);                                                                   \
  }

#define SECTION(string)                                                        \
  (std::cerr << "testing section " << #string << std::endl);

#endif // COMPILER_TEST_HELPERS_HPP
