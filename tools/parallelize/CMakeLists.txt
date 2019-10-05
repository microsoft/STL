# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

add_executable(parallelize parallelize.cpp ../inc/stljobs.h)
target_link_libraries(parallelize Userenv.lib)
