include_directories(../compiler/include)
include_directories(../tests)

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})

find_library(Compiler PATHS "../compiler")

function(add_compiler_test bin_name path)
    message(STATUS "Adding test ${bin_name}")
    if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
        add_compile_options(-Wno-deprecated -Wno-deprecated-declarations)
    endif()
    add_executable(${bin_name} ${path})
    target_link_libraries(${bin_name} PUBLIC Compiler)
    target_compile_options(
        ${bin_name} PRIVATE -fPIC -flto -O3
    )
    add_test(
         NAME ${bin_name}
      COMMAND ${bin_name}
    )
endfunction()

function(DeleteOldInputFiles)
    file(GLOB_RECURSE input_files "${CMAKE_CURRENT_BINARY_DIR}/*.wl")
    foreach (file ${input_files})
        file(REMOVE ${file})
    endforeach()
endfunction()

function(CopyInputFiles)
    file(GLOB_RECURSE input_files "*.wl")
    foreach (file ${input_files})
        get_filename_component(name ${file} NAME_WE)
        message(STATUS "Put test file to: ${CMAKE_CURRENT_BINARY_DIR}/${name}")
        file(
            COPY ${file}
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
        )
    endforeach()
endfunction()

DeleteOldInputFiles()
CopyInputFiles()

file(GLOB_RECURSE test_files "*.cpp")
foreach(file ${test_files})
    get_filename_component(name ${file} NAME_WE)
    add_compiler_test(${name} ${file})
endforeach()