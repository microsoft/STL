# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

project(msvc_standard_libraries_validate LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_compile_definitions(NOMINMAX UNICODE _UNICODE)
add_compile_options(/W4 /WX $<$<NOT:$<CONFIG:Debug>>:/Zi> /permissive-)

add_executable(validate validate.cpp)
