file (GLOB_RECURSE SOURCES *.cpp)
add_library(
    Compiler SHARED "${SOURCES}"
)

target_compile_options(
    Compiler PRIVATE
    -Werror -Wall -Wextra -Wsign-compare -Wshadow -Wwrite-strings
    -Wpointer-arith -Winit-self -Wno-sign-conversion -Wredundant-decls
    -fPIC -flto -O3
)

include_directories(
    Compiler PUBLIC
    include
)