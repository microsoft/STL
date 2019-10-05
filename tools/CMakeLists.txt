# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.15)
project(msvc_standard_libraries_tools LANGUAGES CXX)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED True)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_compile_definitions(NOMINMAX _CRT_RAND_S UNICODE _UNICODE)

if(MSVC)
    add_compile_options(/W4 /WX $<$<NOT:$<CONFIG:Debug>>:/Zi> /std:c++17)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include_directories(inc)

add_subdirectory(jobify)
add_subdirectory(parallelize)
