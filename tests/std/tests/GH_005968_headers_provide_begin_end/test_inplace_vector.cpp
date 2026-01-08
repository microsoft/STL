// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if defined(__cpp_lib_inplace_vector)

#include <inplace_vector>

#include "shared_test.hpp"

void test_inplace_vector() {
    std::inplace_vector<int, 3> container{1, 2, 3};
    shared_test(container);
}

#else // ^^^ defined(__cpp_lib_inplace_vector) / feature not available vvv

void test_inplace_vector() {}

#endif // feature not available
