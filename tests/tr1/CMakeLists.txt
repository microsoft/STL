# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

execute_process(
  COMMAND "${Python_EXECUTABLE}" "${STL_SOURCE_DIR}/tools/scripts/check_test_lst_paths.py" "${CMAKE_CURRENT_SOURCE_DIR}"
  COMMAND_ERROR_IS_FATAL ANY
)

set(TR1_EXPECTED_RESULTS "${CMAKE_CURRENT_SOURCE_DIR}/expected_results.txt")
set(TR1_TEST_OUTPUT_DIR "${STL_TEST_OUTPUT_DIR}/tr1")

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg)

set(TR1_LIT_CONFIG_MAP "map_config(\"${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg\", \"${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg\")\n")
set_property(GLOBAL APPEND_STRING PROPERTY STL_LIT_CONFIG_MAP ${TR1_LIT_CONFIG_MAP})
set_property(GLOBAL APPEND PROPERTY STL_LIT_TEST_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
