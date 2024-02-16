# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.28.0)
project(msvc_standard_libraries_validate LANGUAGES CXX)

add_executable(validate-binary validate.cpp)
# we use SAL annotations, so pass /analyze
target_compile_options(validate-binary PRIVATE /W4 /WX /analyze)
set_target_properties(validate-binary
    PROPERTIES
        CXX_STANDARD 23
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
        # statically link the standard library
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

add_custom_target(run-validate
    COMMAND validate-binary
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../.."
)
