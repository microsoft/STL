// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if __has_include(<inplace_vector>)

#include <inplace_vector>

#include "shared_test.hpp"

void test_inplace_vector() {
    std::inplace_vector<int, 3> container{1, 2, 3};
    shared_test(container);
}

#else // ^^^ __has_include(<inplace_vector>) / !__has_include(<inplace_vector>) vvv

void test_inplace_vector() {}

#endif // !__has_include(<inplace_vector>)