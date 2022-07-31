# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(STL_BENCHMARK_MSVC_RUNTIME_LIBRARY
    MultiThreaded
    CACHE STRING "The flavor of the standard library to use; see https://cmake.org/cmake/help/latest/variable/CMAKE_MSVC_RUNTIME_LIBRARY.html for more information.")
set_property(CACHE STL_BENCHMARK_MSVC_RUNTIME_LIBRARY
    PROPERTY STRINGS
    "MultiThreaded;MultiThreadedDLL;MultiThreadedDebug;MultiThreadedDebugDLL"
)
set(CMAKE_MSVC_RUNTIME_LIBRARY "${STL_BENCHMARK_MSVC_RUNTIME_LIBRARY}")

set(STL_BENCHMARK_ITERATOR_DEBUG_LEVEL
    default
    CACHE STRING "What level of iterator debugging to use."
)
set_property(CACHE STL_BENCHMARK_ITERATOR_DEBUG_LEVEL
    PROPERTY STRINGS
    "default;0;1;2"
)

if(NOT STL_BENCHMARK_ITERATOR_DEBUG_LEVEL STREQUAL "default")
    add_compile_definitions("_ITERATOR_DEBUG_LEVEL=${STL_BENCHMARK_ITERATOR_DEBUG_LEVEL}")
endif()

set(CMAKE_BUILD_TYPE RelWithDebInfo)

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/google-benchmark/.git")
    message(FATAL_ERROR "google-benchmark is not checked out; make sure to run\n    git submodule update --init benchmarks/google-benchmark")
endif()

set(BENCHMARK_ENABLE_DOXYGEN OFF)
set(BENCHMARK_ENABLE_INSTALL OFF)
set(BENCHMARK_ENABLE_TESTING OFF)
set(BUILD_SHARED_LIBS OFF)

# TRANSITION, GH-2816: on some machines, librt is found, despite it being a unix-only library
set(HAVE_LIB_RT OFF)

include_directories(BEFORE "${CMAKE_BINARY_DIR}/out/inc")
link_directories(BEFORE "${STL_LIBRARY_OUTPUT_DIRECTORY}")

add_subdirectory(google-benchmark EXCLUDE_FROM_ALL)

set(benchmark_headers
    "inc/udt.hpp"
    "inc/utility.hpp"
    "inc/xoshiro.hpp"
)

function(add_benchmark name)
    cmake_parse_arguments(PARSE_ARGV 1 "arg" "" "CXX_STANDARD" "")

    if(NOT DEFINED arg_CXX_STANDARD)
        set(arg_CXX_STANDARD 23)
    elseif(NOT arg_CXX_STANDARD MATCHES "^[0-9][0-9]$")
        message(FATAL_ERROR "Unexpected value for CXX_STANDARD: ${arg_CXX_STANDARD}")
    endif()

    if(NOT DEFINED arg_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "benchmark ${name} does not have any source files")
    endif()

    add_executable(benchmark-${name}
        ${benchmark_headers}
        ${arg_UNPARSED_ARGUMENTS}
    )

    target_compile_features(benchmark-${name} PRIVATE cxx_std_${arg_CXX_STANDARD})
    target_include_directories(benchmark-${name} PRIVATE inc)
    target_link_libraries(benchmark-${name} PRIVATE benchmark::benchmark)
endfunction()

add_benchmark(std_copy
    src/std_copy.cpp
    CXX_STANDARD 23
)
