file (GLOB_RECURSE SOURCES *.cpp)
add_library(
    Compiler SHARED "${SOURCES}"
)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(Compiler PRIVATE ${llvm_libs})

target_compile_options(
    Compiler PRIVATE
    -Wall -Wextra -Wpedantic -Wsign-compare -Wwrite-strings
    -Wimplicit-fallthrough -Wunused
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