cmake_minimum_required(VERSION 3.9)

project(
    Weak_Compiler
    DESCRIPTION "The compiler for unnamed programming language"
    HOMEPAGE_URL "https://github.com/epoll-reactor/weak_compiler"
    LANGUAGES C CXX
)

set(CMAKE_C_COMPILER /bin/gcc)
set(CMAKE_CXX_COMPILER /bin/g++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

enable_testing()

add_subdirectory(compiler)
add_subdirectory(tests)
