# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

get_property(STL_LIT_CONFIG_MAP GLOBAL PROPERTY STL_LIT_CONFIG_MAP)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/stl-lit.in
    ${STL_LIT}
)
