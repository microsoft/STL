// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if defined(__cpp_lib_flat_set)
static_assert(false, "When this feature is implemented, update this to a Standard mode check.");

#include <flat_set>

#include "shared_test.hpp"

void test_flat_set() {
    {
        std::flat_set<int> container{1, 2, 3};
        shared_test(container);
    }

    {
        std::flat_multiset<int> container2{1, 2, 3};
        shared_test(container2);
    }
}

#else // ^^^ defined(__cpp_lib_flat_set) / !defined(__cpp_lib_flat_set) vvv

void test_flat_set() {}

#endif // ^^^ !defined(__cpp_lib_flat_set) ^^^
