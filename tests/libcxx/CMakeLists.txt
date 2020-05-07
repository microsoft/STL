# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(LIBCXX_ENVLST "${CMAKE_CURRENT_SOURCE_DIR}/usual_matrix.lst")
set(LIBCXX_EXPECTED_RESULTS "${CMAKE_CURRENT_SOURCE_DIR}/expected_results.txt")
set(LIBCXX_SOURCE_DIR "${LLVM_PROJECT_SOURCE_DIR}/libcxx" CACHE PATH
    "Location of the libcxx source tree")
set(LIBCXX_TEST_OUTPUT_DIR "${STL_TEST_OUTPUT_DIR}/libcxx")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg)

set(LIBCXX_LIT_CONFIG_MAP "map_config(\"${LIBCXX_SOURCE_DIR}/test/lit.cfg\", \"${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg\")\n")
set_property(GLOBAL APPEND_STRING PROPERTY STL_LIT_CONFIG_MAP ${LIBCXX_LIT_CONFIG_MAP})
set_property(GLOBAL APPEND PROPERTY STL_LIT_TEST_DIRS "${LIBCXX_SOURCE_DIR}/test/std")
