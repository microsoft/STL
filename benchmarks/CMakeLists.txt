# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(STL_BENCHMARK_FLAVOR_DYNAMIC_OR_STATIC
    static
    CACHE STRING "Whether to use the dynamic or static standard library"
)
set(STL_BENCHMARK_FLAVOR_DEBUG_OR_RELEASE
    release
    CACHE STRING "Whether to use the debug or release library"
)
set(STL_BENCHMARK_FLAVOR_ITERATOR_DEBUGGING
    default
    CACHE STRING "What level of iterator debugging to use"
)

if(STL_BENCHMARK_FLAVOR_DYNAMIC_OR_STATIC MATCHES "^[Dd][Yy][Nn][Aa][Mm][Ii][Cc]$")
    set(stl_is_dynamic ON)
elseif(STL_BENCHMARK_FLAVOR_DYNAMIC_OR_STATIC MATCHES "^[Ss][Tt][Aa][Tt][Ii][Cc]$")
    set(stl_is_dynamic OFF)
else()
    message(FATAL_ERROR
        "Invalid setting for STL_BENCHMARK_FLAVOR_DYNAMIC_OR_STATIC (${STL_BENCHMARK_FLAVOR_DYNAMIC_OR_STATIC})\n"
        "    expected one of 'dynamic', 'static'"
    )
endif()

if(STL_BENCHMARK_FLAVOR_DEBUG_OR_RELEASE MATCHES "^[Rr][Ee][Ll][Ee][Aa][Ss][Ee]$")
    set(stl_is_release ON)
elseif(STL_BENCHMARK_FLAVOR_DEBUG_OR_RELEASE MATCHES "^[Dd][Ee][Bb][Uu][Gg]$")
    set(stl_is_release OFF)
else()
    message(FATAL_ERROR
        "Invalid setting for STL_BENCHMARK_FLAVOR_DEBUG_OR_RELEASE (${STL_BENCHMARK_FLAVOR_DEBUG_OR_RELEASE})\n"
        "    expected one of 'release', 'debug'"
    )
endif()

if(stl_is_release)
    set(debug_flavor "")
else()
    set(debug_flavor "d")
endif()

set(iterator_debugging "")
if(STL_BENCHMARK_FLAVOR_ITERATOR_DEBUGGING MATCHES "^[Dd][Ee][Ff][Aa][Uu][Ll][Tt]$")
    # default
elseif(STL_BENCHMARK_FLAVOR_ITERATOR_DEBUGGING STREQUAL "0")
    if(NOT stl_is_release)
        set(iterator_debugging "0")
    endif()
elseif(STL_BENCHMARK_FLAVOR_ITERATOR_DEBUGGING STREQUAL "1")
    set(iterator_debugging "1")
elseif(STL_BENCHMARK_FLAVOR_ITERATOR_DEBUGGING STREQUAL "2")
    if(stl_is_release)
        message(FATAL_ERROR "The release libraries do not support full iterator debugging")
    endif()
else()
    message(FATAL_ERROR
        "Invalid setting for STL_BENCHMARK_FLAVOR_ITERATOR_DEBUGGING (${STL_BENCHMARK_FLAVOR_ITERATOR_DEBUGGING})\n"
        "    expected one of 'default', '0', '1', 2'"
    )
endif()

if(stl_is_dynamic)
    if(NOT iterator_debugging STREQUAL "")
        message(FATAL_ERROR "The dynamic STL does not support non-default iterator debugging")
    endif()
    set(cxx_library "msvcp${debug_flavor}_implib")
    set(c_library "msvcrt${debug_flavor}")
else()
    set(cxx_library "libcpmt${debug_flavor}${iterator_debugging}")
    set(c_library "libcmt${debug_flavor}")
endif()

if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/google-benchmark")
    message(FATAL_ERROR "google-benchmark is not checked out; make sure to run `git submodule update --init benchmarks/google-benchmark`")
endif()

set(BENCHMARK_ENABLE_DOXYGEN OFF)
set(BENCHMARK_ENABLE_INSTALL OFF)
set(BENCHMARK_ENABLE_TESTING OFF)
set(HAVE_LIB_RT OFF) # for some reason, check_library_exists(rt) succeeds and it should not
add_subdirectory(google-benchmark)
target_link_libraries(benchmark PUBLIC "${cxx_library}" "${c_library}")
target_compile_options(benchmark PRIVATE ${VCLIBS_RELEASE_OPTIONS} /EHsc)
target_compile_options(benchmark_main PRIVATE ${VCLIBS_RELEASE_OPTIONS} /EHsc)

file(GLOB benchmark_sources "src/*.cpp")

add_executable(stl-benchmark
    ${benchmark_sources}
)
target_include_directories(stl-benchmark PRIVATE inc)
target_link_libraries(stl-benchmark PRIVATE benchmark::benchmark benchmark::benchmark_main)
target_compile_options(stl-benchmark PRIVATE ${VCLIBS_RELEASE_OPTIONS} /EHsc)

export(
  TARGETS ${${cxx_library}_all_targets}
  NAMESPACE "msvc_standard_library"
  FILE "${CMAKE_CURRENT_BINARY_DIR}/stl-phony-targets-for-debug.cmake"
)
