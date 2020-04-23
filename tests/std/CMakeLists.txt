# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(STD_EXPECTED_RESULTS "${CMAKE_CURRENT_SOURCE_DIR}/expected_results.txt")
set(STD_TEST_OUTPUT_DIR "${STL_TEST_OUTPUT_DIR}/std")
set(STD_TEST_SUBDIRS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/test.lst")
set(STD_TEST_SUBDIRS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg)

get_property(STL_LIT_CONFIG_MAP GLOBAL PROPERTY STL_LIT_CONFIG_MAP)
string(APPEND STL_LIT_CONFIG_MAP "map_config(\"${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg\", \"${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg\")\n")
set_property(GLOBAL PROPERTY STL_LIT_CONFIG_MAP ${STL_LIT_CONFIG_MAP})

if(ENABLE_XUNIT_OUTPUT)
    list(APPEND STD_ADDITIONAL_LIT_FLAGS "--xunit-xml-output" "${CMAKE_CURRENT_BINARY_DIR}/std.test.xml")
endif()

list(APPEND STD_STL_LIT_COMMAND "${STL_LIT}"
                                "${ADDITIONAL_LIT_FLAGS}"
                                "${STD_ADDITIONAL_LIT_FLAGS}"
                                "${CMAKE_CURRENT_SOURCE_DIR}")

add_test(NAME std COMMAND ${Python3_EXECUTABLE} ${STD_STL_LIT_COMMAND} COMMAND_EXPAND_LISTS)
set_tests_properties(std PROPERTIES RUN_SERIAL TRUE)
