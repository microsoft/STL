# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(STL_BENCHMARK_MSVC_RUNTIME_LIBRARY
    MultiThreaded
    CACHE STRING "The version of the standard library to use; see https://cmake.org/cmake/help/latest/variable/CMAKE_MSVC_RUNTIME_LIBRARY.html for more information.")
set_property(CACHE STL_BENCHMARK_MSVC_RUNTIME_LIBRARY
    PROPERTY STRINGS
        "MultiThreaded;MultiThreadedDLL;MultiThreadedDebug;MultiThreadedDebugDLL"
)

set(STL_BENCHMARK_ITERATOR_DEBUG_LEVEL
    default
    CACHE STRING "What level of iterator debugging to use."
)
set_property(CACHE STL_BENCHMARK_ITERATOR_DEBUG_LEVEL
    PROPERTY STRINGS
        "default;0;1;2"
)

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/google-benchmark")
    message(FATAL_ERROR "google-benchmark is not checked out; make sure to run `git submodule update --init benchmarks/google-benchmark`")
endif()

set(BENCHMARK_ENABLE_DOXYGEN OFF)
set(BENCHMARK_ENABLE_INSTALL OFF)
set(BENCHMARK_ENABLE_TESTING OFF)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_MSVC_RUNTIME_LIBRARY "${STL_BENCHMARK_MSVC_RUNTIME_LIBRARY}")
add_subdirectory(google-benchmark)
target_include_directories(benchmark BEFORE PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/out/inc")
target_compile_options(benchmark PRIVATE ${VCLIBS_RELEASE_OPTIONS} /EHsc)
target_compile_options(benchmark_main PRIVATE ${VCLIBS_RELEASE_OPTIONS} /EHsc)
if(NOT STL_BENCHMARK_ITERATOR_DEBUG_LEVEL STREQUAL "default")
    target_compile_definitions(benchmark PUBLIC "-D_ITERATOR_DEBUG_LEVEL=${STL_BENCHMARK_ITERATOR_DEBUG_LEVEL}")
endif()

file(GLOB benchmark_sources "src/*.cpp")

add_executable(stl-benchmark
    ${benchmark_sources}
)
target_include_directories(stl-benchmark PRIVATE inc)
target_link_libraries(stl-benchmark PRIVATE benchmark::benchmark benchmark::benchmark_main)
target_compile_options(stl-benchmark PRIVATE ${VCLIBS_RELEASE_OPTIONS} /EHsc)
target_link_directories(stl-benchmark
    BEFORE
    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/lib/${VCLIBS_I386_OR_AMD64}")
