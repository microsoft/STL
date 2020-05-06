# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

include(ProcessorCount)
ProcessorCount(NUM_PROCESSORS)

add_custom_target(tr1)
add_dependencies(check tr1)
add_custom_target(check-tr1 COMMAND "ctest --progress -j ${NUM_PROCESSORS} --timeout 240 -O \"${CMAKE_CURRENT_BINARY_DIR}/test_results.log\" -I \"${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake\"" DEPENDS tr1 COMMENT "Running tr1 tests" USES_TERMINAL)

include("${CMAKE_CURRENT_BINARY_DIR}/tests.cmake")
