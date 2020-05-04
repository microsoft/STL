# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(STD_TEST_OUTPUT_DIR "${STL_TEST_OUTPUT_DIR}/std")
set(STD_TEST_SUBDIRS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/test.lst")
set(STD_TEST_SUBDIRS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(STD_TEST_TYPE_OVERRIDES "${CMAKE_CURRENT_SOURCE_DIR}/test_type_overrides.txt")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg)

get_property(STL_LIT_CONFIG_MAP GLOBAL PROPERTY STL_LIT_CONFIG_MAP)
string(APPEND STL_LIT_CONFIG_MAP "map_config(\"${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg\", \"${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg\")\n")
set_property(GLOBAL PROPERTY STL_LIT_CONFIG_MAP ${STL_LIT_CONFIG_MAP})
