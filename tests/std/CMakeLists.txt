# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

include(ProcessorCount)
ProcessorCount(NUM_PROCESSORS)

add_custom_target(std)
add_dependencies(check std)
add_custom_target(check-std COMMAND "ctest --progress -j ${NUM_PROCESSORS} --timeout 240 -O \"${CMAKE_CURRENT_BINARY_DIR}/test_results.log\" -I \"${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake\"" DEPENDS std COMMENT "Running std tests" USES_TERMINAL)

include("${CMAKE_CURRENT_BINARY_DIR}/tests.cmake")
