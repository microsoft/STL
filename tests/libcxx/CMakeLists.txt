# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(LIBCXX_ENVLST "${CMAKE_CURRENT_SOURCE_DIR}/usual_matrix.lst")
set(LIBCXX_EXPECTED_RESULTS "${CMAKE_CURRENT_SOURCE_DIR}/expected_results.txt")
set(LIBCXX_EXPECTED_RESULTS_ROOT "${LIBCXX_SOURCE_DIR}/test/std")
set(LIBCXX_SKIP_LIST "${CMAKE_CURRENT_SOURCE_DIR}/skipped_tests.txt")
set(LIBCXX_SKIP_LIST_ROOT "${LIBCXX_SOURCE_DIR}/test/std")
set(LIBCXX_TEST_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg)

get_property(LLVM_LIT_CONFIG_MAP GLOBAL PROPERTY LLVM_LIT_CONFIG_MAP)
string(APPEND LLVM_LIT_CONFIG_MAP "map_config(\"${LIBCXX_SOURCE_DIR}/test/lit.cfg\", \"${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg\")\n")
set_property(GLOBAL PROPERTY LLVM_LIT_CONFIG_MAP ${LLVM_LIT_CONFIG_MAP})

if(ENABLE_XUNIT_OUTPUT)
    list(APPEND LIBCXX_ADDITIONAL_LIT_FLAGS "--xunit-xml-output" "${CMAKE_CURRENT_BINARY_DIR}/libcxx.test.xml")
endif()

list(APPEND LIBCXX_LLVM_LIT_COMMAND "${LLVM_LIT_LOCATION}"
                                    "${ADDITIONAL_LIT_FLAGS}"
                                    "${LIBCXX_ADDITIONAL_LIT_FLAGS}"
                                    "${CMAKE_CURRENT_BINARY_DIR}")

add_test(NAME libcxx COMMAND ${Python3_EXECUTABLE} ${LIBCXX_LLVM_LIT_COMMAND} COMMAND_EXPAND_LISTS)
set_tests_properties(libcxx PROPERTIES RUN_SERIAL TRUE)
