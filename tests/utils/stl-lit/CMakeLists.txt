# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(LLVM_SOURCE_DIR "${LLVM_PROJECT_SOURCE_DIR}/llvm" CACHE PATH
    "Location of the llvm source tree")

set(STL_LIT_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/stl-lit.py")
set(STL_LIT_OUTPUT "${STL_LIT_OUTPUT}" PARENT_SCOPE)

get_property(STL_LIT_CONFIG_MAP GLOBAL PROPERTY STL_LIT_CONFIG_MAP)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/stl-lit.in"
    "${STL_LIT_OUTPUT}")
