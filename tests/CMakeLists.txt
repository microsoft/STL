# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(STL_SOURCE_DIR "${PROJECT_SOURCE_DIR}" CACHE PATH
    "Location of the stl source tree")
set(STL_TEST_UTILS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/utils")
set(STL_OUTPUT_DIR "${PROJECT_BINARY_DIR}/out")
set(STL_OUTPUT_HEADERS_DIR "${STL_OUTPUT_DIR}/inc")

set(LLVM_PROJECT_SOURCE_DIR "${STL_SOURCE_DIR}/llvm-project" CACHE PATH
    "Location of the llvm-project source tree")
# This being a cache variable serves the dual purpose of letting the user point
# to different out of tree LLVM components and also to override as set() call
# in llvm-lit's CMakeLists.txt.
set(LLVM_SOURCE_DIR "${LLVM_PROJECT_SOURCE_DIR}/llvm" CACHE PATH
    "Location of the llvm source tree")
set(LLVM_LIT_SOURCE_DIR "${LLVM_SOURCE_DIR}/utils/llvm-lit")
set(LLVM_LIT_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/llvm-lit")

list(APPEND CMAKE_MODULE_PATH "${LLVM_SOURCE_DIR}/cmake/modules")
include(AddLLVM) # for get_llvm_lit_path needed in llvm-lit's CMakeLists.txt

# The result of the first call to get_llvm_lit_path is cached
get_llvm_lit_path(LIT_BASE_DIR LIT_FILE_NAME)
set(LLVM_LIT_LOCATION "${LIT_BASE_DIR}/${LIT_FILE_NAME}")

set(Python_FIND_STRATEGY VERSION)
find_package(Python3)

add_subdirectory(libcxx)

# Add the llvm-lit subdirectory last so all the test directories have had a
# chance to add to the config map.
add_subdirectory(${LLVM_LIT_SOURCE_DIR} llvm-lit)
