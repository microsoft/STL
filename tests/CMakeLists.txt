# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(STL_BUILD_ROOT "${PROJECT_BINARY_DIR}/out")
set(STL_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(STL_TEST_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(STL_TEST_UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/utils")
set(STL_TESTED_HEADERS_DIR "${STL_BUILD_ROOT}/inc")

set(LLVM_PROJECT_SOURCE_DIR "${STL_SOURCE_DIR}/llvm-project" CACHE PATH
    "Location of the llvm-project source tree")
set(LIBCXX_SOURCE_DIR "${LLVM_PROJECT_SOURCE_DIR}/libcxx" CACHE PATH
    "Location of the libcxx source tree")
set(LLVM_SOURCE_DIR "${LLVM_PROJECT_SOURCE_DIR}/llvm" CACHE PATH
    "Location of the llvm source tree")

add_subdirectory(libcxx)
add_subdirectory(std)
add_subdirectory(tr1)

set(STL_LIT ${CMAKE_CURRENT_BINARY_DIR}/utils/stl-lit/stl-lit.py)
# Add the stl-lit subdirectory last so all the test directories have had a
# chance to add to the config map.
add_subdirectory(utils/stl-lit)

set(Python_FIND_STRATEGY VERSION)
find_package(Python3)

execute_process(
    COMMAND ${Python3_EXECUTABLE}
            ${STL_LIT}
            "${LIBCXX_SOURCE_DIR}/test/std"
            "${CMAKE_CURRENT_SOURCE_DIR}/std"
            "${CMAKE_CURRENT_SOURCE_DIR}/tr1")

include("${CMAKE_CURRENT_BINARY_DIR}/libcxx/tests.cmake")
include("${CMAKE_CURRENT_BINARY_DIR}/std/tests.cmake")
include("${CMAKE_CURRENT_BINARY_DIR}/tr1/tests.cmake")

get_property(STL_LIT_GENERATED_FILES GLOBAL PROPERTY STL_LIT_GENERATED_FILES)

#add_custom_target(
#    generate_test_files
#    ${Python3_EXECUTABLE}
#    ${STL_LIT}
#    #"${LIBCXX_SOURCE_DIR}/test/std"
#    #"${CMAKE_CURRENT_SOURCE_DIR}/std"
#    "${CMAKE_CURRENT_SOURCE_DIR}/tr1"
#    BYPRODUCTS ${STL_LIT_GENERATED_FILES})
