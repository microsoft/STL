# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.22)
project(msvc_standard_libraries_validate LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_executable(validate-binary validate.cpp)
# we use SAL annotations, so pass /analyze
target_compile_options(validate-binary PRIVATE /W4 /WX $<$<NOT:$<CONFIG:Debug>>:/Zi> /permissive- /analyze /EHsc)
target_compile_definitions(validate-binary PRIVATE NOMINMAX UNICODE _UNICODE)

add_custom_target(run-validate
    COMMAND validate-binary
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../.."
)
