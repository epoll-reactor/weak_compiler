file (GLOB_RECURSE SOURCES *.cpp)
add_library(
    Compiler SHARED "${SOURCES}"
)

target_compile_options(
    Compiler PRIVATE
    -Werror -Wall -Wextra -Wpedantic -Wsign-compare -Wshadow -Wwrite-strings
    -Wimplicit-fallthrough
    -Wpointer-arith -Winit-self -Wno-sign-conversion -Wredundant-decls
    -fPIC -flto -O3
)

if (WEAK_COMPILER_SANITIZE)
    message(STATUS "Building the compiler library with sanitizer flags")
    add_compile_options(
      -fsanitize=address -fno-omit-frame-pointer
      -fsanitize=undefined -fno-sanitize-recover=all
      -fsanitize-address-use-after-scope
    )
    link_libraries(
      -fsanitize=address -fno-omit-frame-pointer
      -fsanitize=undefined -fno-sanitize-recover=all
      -fsanitize-address-use-after-scope
    )
    if(CMAKE_CXX_COMPILER MATCHES "clang")
      add_compile_options(
        -fsanitize=cfi -fvisibility=default -flto
      )
      link_libraries(
        -fsanitize=cfi -fvisibility=default -flto
      )
    endif()
endif()

include_directories(
    Compiler PUBLIC
    include
)