// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if defined(__cpp_lib_inplace_vector)
static_assert(false, "When this feature is implemented, update this to a Standard mode check.");

#include <inplace_vector>

#include "shared_test.hpp"

void test_inplace_vector() {
    std::inplace_vector<int, 3> container{1, 2, 3};
    shared_test(container);
}

#else // ^^^ defined(__cpp_lib_inplace_vector) / !defined(__cpp_lib_inplace_vector) vvv

void test_inplace_vector() {}

#endif // ^^^ !defined(__cpp_lib_inplace_vector) ^^^
