# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(LIBCXX_ENVLST "${CMAKE_CURRENT_SOURCE_DIR}/usual_matrix.lst")
set(LIBCXX_EXPECTED_RESULTS "${CMAKE_CURRENT_SOURCE_DIR}/expected_results.txt")
set(LIBCXX_TEST_OUTPUT_DIR "${STL_TEST_OUTPUT_DIR}/libcxx")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg)

get_property(STL_LIT_CONFIG_MAP GLOBAL PROPERTY STL_LIT_CONFIG_MAP)
string(APPEND STL_LIT_CONFIG_MAP "map_config(\"${LIBCXX_SOURCE_DIR}/test/lit.cfg\", \"${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg\")\n")
set_property(GLOBAL PROPERTY STL_LIT_CONFIG_MAP ${STL_LIT_CONFIG_MAP})

if(ENABLE_XUNIT_OUTPUT)
    list(APPEND LIBCXX_ADDITIONAL_LIT_FLAGS "--xunit-xml-output" "${CMAKE_CURRENT_BINARY_DIR}/libcxx.test.xml")
endif()

list(APPEND LIBCXX_STL_LIT_COMMAND "${STL_LIT}"
                                   "${ADDITIONAL_LIT_FLAGS}"
                                   "${LIBCXX_ADDITIONAL_LIT_FLAGS}"
                                   "${LIBCXX_SOURCE_DIR}/test")

add_test(NAME libcxx COMMAND ${Python3_EXECUTABLE} ${LIBCXX_STL_LIT_COMMAND} COMMAND_EXPAND_LISTS)
set_tests_properties(libcxx PROPERTIES RUN_SERIAL TRUE)
