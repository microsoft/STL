# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.31.0)
project(msvc_standard_libraries_benchmarks LANGUAGES CXX)

if(DEFINED STL_BINARY_DIR)
    cmake_path(ABSOLUTE_PATH STL_BINARY_DIR
        BASE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.."
        NORMALIZE
    )
    if(NOT EXISTS "${STL_BINARY_DIR}/out")
        message(FATAL_ERROR "Invalid STL_BINARY_DIR '${STL_BINARY_DIR}'")
    endif()

    if(NOT DEFINED VCLIBS_TARGET_ARCHITECTURE)
        set(VCLIBS_TARGET_ARCHITECTURE "${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
    endif()

    string(TOLOWER "${VCLIBS_TARGET_ARCHITECTURE}" VCLIBS_TARGET_ARCHITECTURE)

    if("${VCLIBS_TARGET_ARCHITECTURE}" STREQUAL "x86")
        set(VCLIBS_I386_OR_AMD64 "i386")
    elseif(VCLIBS_TARGET_ARCHITECTURE STREQUAL "x64")
        set(VCLIBS_I386_OR_AMD64 "amd64")
    elseif(VCLIBS_TARGET_ARCHITECTURE STREQUAL "arm64")
        set(VCLIBS_I386_OR_AMD64 "arm64")
    elseif(VCLIBS_TARGET_ARCHITECTURE STREQUAL "arm64ec")
        set(VCLIBS_I386_OR_AMD64 "arm64ec")
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/arm64EC>)
        add_link_options("/machine:arm64ec")
        set(CMAKE_STATIC_LINKER_FLAGS "/machine:arm64ec")
    else()
        message(FATAL_ERROR "Could not determine target architecture: VCLIBS_TARGET_ARCHITECTURE: ${VCLIBS_TARGET_ARCHITECTURE}")
    endif()

    include_directories(BEFORE "${STL_BINARY_DIR}/out/inc")
    link_directories(BEFORE "${STL_BINARY_DIR}/out/lib/${VCLIBS_I386_OR_AMD64}")
else()
    message(WARNING "STL_BINARY_DIR not set; benchmarking the globally installed standard library")
endif()

set(STL_BENCHMARK_MSVC_RUNTIME_LIBRARY
    MultiThreaded
    CACHE STRING "The flavor of the standard library to use; see https://cmake.org/cmake/help/latest/variable/CMAKE_MSVC_RUNTIME_LIBRARY.html for more information.")
set_property(CACHE STL_BENCHMARK_MSVC_RUNTIME_LIBRARY
    PROPERTY STRINGS
    "MultiThreaded;MultiThreadedDLL;MultiThreadedDebug;MultiThreadedDebugDLL"
)
set(CMAKE_MSVC_RUNTIME_LIBRARY "${STL_BENCHMARK_MSVC_RUNTIME_LIBRARY}")

# Building the benchmarks as Release optimizes them with `/O2 /Ob2`.
# Compiling with `/Zi` and linking with `/DEBUG` below makes profiling possible.
# (RelWithDebInfo would use `/O2 /Ob1 /Zi`.) See GH-4496.
set(CMAKE_BUILD_TYPE Release)

# /utf-8 affects <format>.
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/Zi;/nologo;/diagnostics:caret;/W4;/WX;/w14265;/w15038;/w15262;/utf-8>")
add_compile_options("$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/Zc:preprocessor>") # TRANSITION, LLVM-48220 clang-cl: ignore /Zc:preprocessor

add_link_options("/DEBUG")

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/google-benchmark/.git")
    message(FATAL_ERROR "google-benchmark is not checked out; make sure to run\n    git submodule update --init benchmarks/google-benchmark")
endif()

set(BENCHMARK_ENABLE_DOXYGEN OFF)
set(BENCHMARK_ENABLE_INSTALL OFF)
set(BENCHMARK_ENABLE_TESTING OFF)

add_subdirectory(google-benchmark EXCLUDE_FROM_ALL)

set(benchmark_headers
    "inc/lorem.hpp"
    "inc/skewed_allocator.hpp"
    "inc/udt.hpp"
    "inc/utility.hpp"
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

add_benchmark(adjacent_difference src/adjacent_difference.cpp)
add_benchmark(adjacent_find src/adjacent_find.cpp)
add_benchmark(any_swap src/any_swap.cpp)
add_benchmark(bitset_from_string src/bitset_from_string.cpp)
add_benchmark(bitset_to_string src/bitset_to_string.cpp)
add_benchmark(efficient_nonlocking_print src/efficient_nonlocking_print.cpp)
add_benchmark(filesystem src/filesystem.cpp)
add_benchmark(fill src/fill.cpp)
add_benchmark(find_and_count src/find_and_count.cpp)
add_benchmark(find_first_of src/find_first_of.cpp)
add_benchmark(has_single_bit src/has_single_bit.cpp)
add_benchmark(includes src/includes.cpp)
add_benchmark(iota src/iota.cpp)
add_benchmark(is_sorted_until src/is_sorted_until.cpp)
add_benchmark(locale_classic src/locale_classic.cpp)
add_benchmark(locate_zone src/locate_zone.cpp)
add_benchmark(minmax_element src/minmax_element.cpp)
add_benchmark(mismatch src/mismatch.cpp)
add_benchmark(move_only_function src/move_only_function.cpp)
add_benchmark(nth_element src/nth_element.cpp)
add_benchmark(path_lexically_normal src/path_lexically_normal.cpp)
add_benchmark(priority_queue_push_range src/priority_queue_push_range.cpp)
add_benchmark(random_integer_generation src/random_integer_generation.cpp)
add_benchmark(ranges_div_ceil src/ranges_div_ceil.cpp)
add_benchmark(regex_search src/regex_search.cpp)
add_benchmark(remove src/remove.cpp)
add_benchmark(replace src/replace.cpp)
add_benchmark(reverse src/reverse.cpp)
add_benchmark(rotate src/rotate.cpp)
add_benchmark(sample src/sample.cpp)
add_benchmark(search src/search.cpp)
add_benchmark(search_n src/search_n.cpp)
add_benchmark(shuffle src/shuffle.cpp)
add_benchmark(std_copy src/std_copy.cpp)
add_benchmark(sv_equal src/sv_equal.cpp)
add_benchmark(swap_ranges src/swap_ranges.cpp)
add_benchmark(unique src/unique.cpp)
add_benchmark(vector_bool_copy src/vector_bool_copy.cpp)
add_benchmark(vector_bool_copy_n src/vector_bool_copy_n.cpp)
add_benchmark(vector_bool_count src/vector_bool_count.cpp)
add_benchmark(vector_bool_move src/vector_bool_move.cpp)
