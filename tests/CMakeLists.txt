# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(STL_BUILD_ROOT "${PROJECT_BINARY_DIR}/out")
set(STL_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(STL_TEST_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set(STL_TEST_UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/utils")
set(STL_TESTED_HEADERS_DIR "${STL_BUILD_ROOT}/inc")

set(LLVM_PROJECT_SOURCE_DIR "${STL_SOURCE_DIR}/llvm-project" CACHE PATH
    "Location of the llvm-project source tree")
# This being a cache variable serves the dual purpose of letting the user point
# to different out-of-tree LLVM components and also override the set() call
# in llvm-lit's CMakeLists.txt.
set(LLVM_SOURCE_DIR "${LLVM_PROJECT_SOURCE_DIR}/llvm" CACHE PATH
    "Location of the llvm source tree")
set(LLVM_LIT_SOURCE_DIR "${LLVM_SOURCE_DIR}/utils/llvm-lit")
set(LLVM_LIT_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/llvm-lit")

# We need to define this here as opposed to the libcxx CMakeLists.txt as
# the std testsuite also needs to know where libcxx lives
set(LIBCXX_SOURCE_DIR "${LLVM_PROJECT_SOURCE_DIR}/libcxx")

list(APPEND CMAKE_MODULE_PATH "${LLVM_SOURCE_DIR}/cmake/modules")
include(AddLLVM) # for get_llvm_lit_path needed in llvm-lit's CMakeLists.txt

# The result of the first call to get_llvm_lit_path is cached
get_llvm_lit_path(LIT_BASE_DIR LIT_FILE_NAME)
set(LLVM_LIT_LOCATION "${LIT_BASE_DIR}/${LIT_FILE_NAME}")

set(Python_FIND_STRATEGY VERSION)
find_package(Python3)

add_subdirectory(libcxx)
add_subdirectory(std)
add_subdirectory(tr1)

# Add the llvm-lit subdirectory last so all the test directories have had a
# chance to add to the config map.
add_subdirectory(${LLVM_LIT_SOURCE_DIR} llvm-lit)
