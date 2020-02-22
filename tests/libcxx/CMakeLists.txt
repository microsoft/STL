# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(LIBCXX_SOURCE_DIR "${LLVM_PROJECT_SOURCE_DIR}/libcxx" CACHE PATH
    "Location of the libcxx sources")
set(LIBCXX_ENVLST "${CMAKE_CURRENT_SOURCE_DIR}/usual_matrix.lst")
set(LIBCXX_SKIP_LIST "${CMAKE_CURRENT_SOURCE_DIR}/skipped_tests.txt")
set(LIBCXX_SKIP_LIST_ROOT "${LIBCXX_SOURCE_DIR}/test/std")
set(LIBCXX_EXPECTED_RESULTS "${CMAKE_CURRENT_SOURCE_DIR}/expected_results.txt")
set(LIBCXX_EXPECTED_RESULTS_ROOT "${LIBCXX_SOURCE_DIR}/test/std")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg)

get_property(LLVM_LIT_CONFIG_MAP GLOBAL PROPERTY LLVM_LIT_CONFIG_MAP)
string(APPEND LLVM_LIT_CONFIG_MAP "map_config(\"${LIBCXX_SOURCE_DIR}/test/lit.cfg\", \"${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg\")\n")
set_property(GLOBAL PROPERTY LLVM_LIT_CONFIG_MAP ${LLVM_LIT_CONFIG_MAP})

if(ENABLE_XUNIT_OUTPUT)
    string(APPEND LIBCXX_ADDITIONAL_LIT_FLAGS "--xunit-xml-output ${CMAKE_CURRENT_BINARY_DIR}/libcxx.test.xml")
endif()

add_test(NAME libcxx COMMAND ${Python3_EXECUTABLE}
    ${LLVM_LIT_LOCATION}
    ${ADDITIONAL_LIT_FLAGS}
    ${LIBCXX_ADDITIONAL_LIT_FLAGS}
    ${CMAKE_CURRENT_BINARY_DIR})
set_tests_properties(libcxx PROPERTIES RUN_SERIAL TRUE)
