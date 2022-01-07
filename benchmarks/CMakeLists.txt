# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

project(msvc_standard_libraries_benchmarks LANGUAGES CXX)

# Benchmark Executable
find_package(benchmark REQUIRED)
file(GLOB_RECURSE msvc_standard_libraries_benchmarks.sources CONFIGURE_DEPENDS std/**)

set(VCLIBS_BENCHMARKS_OPTIONS
    $<$<CONFIG:Debug>: /Od;/MDd>
    $<$<CONFIG:Release>: /O2;/MD>
)

add_executable(msvc_standard_libraries_benchmarks ${msvc_standard_libraries_benchmarks.sources})
target_include_directories(msvc_standard_libraries_benchmarks SYSTEM PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/../out/inc
)
target_link_libraries(msvc_standard_libraries_benchmarks PRIVATE
    benchmark::benchmark
    ${CMAKE_DL_LIBS}
)
target_compile_options(msvc_standard_libraries_benchmarks PRIVATE
    /std:c++latest
    /EHsc
    /utf-8
    /permissive-
    /W4
    ${VCLIBS_BENCHMARKS_OPTIONS}
)
