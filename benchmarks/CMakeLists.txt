# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

project(stl_benchmarks LANGUAGES CXX)

# Benchmark Executable
find_package(benchmark REQUIRED)
file(GLOB_RECURSE stl_benchmarks.sources CONFIGURE_DEPENDS std/**)

set(VCLIBS_BENCHMARKS_OPTIONS
    $<$<CONFIG:Debug>: /Od;/MDd>
    $<$<CONFIG:Release>: /O2;/MD>
)

add_executable(stl_benchmarks ${stl_benchmarks.sources})
target_include_directories(stl_benchmarks SYSTEM PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/../out/inc
)
target_link_libraries(stl_benchmarks PRIVATE
    benchmark::benchmark
    ${CMAKE_DL_LIBS}
)
target_compile_options(stl_benchmarks PRIVATE
    /std:c++latest
    /EHsc
    /utf-8
    /permissive-
    /W4
    ${VCLIBS_BENCHMARKS_OPTIONS}
)
