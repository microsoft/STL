# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(TR1_EXPECTED_RESULTS "${CMAKE_CURRENT_SOURCE_DIR}/expected_results.txt")
set(TR1_TEST_OUTPUT_DIR "${STL_TEST_OUTPUT_DIR}/tr1")
set(TR1_TEST_SUBDIRS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/test.lst")
set(TR1_TEST_SUBDIRS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg)

set(TR1_LIT_CONFIG_MAP "map_config(\"${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg\", \"${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg\")\n")
set_property(GLOBAL APPEND_STRING PROPERTY STL_LIT_CONFIG_MAP ${TR1_LIT_CONFIG_MAP})
set_property(GLOBAL APPEND PROPERTY STL_LIT_TEST_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
