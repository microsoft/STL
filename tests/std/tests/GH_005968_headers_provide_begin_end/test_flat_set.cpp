// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if _HAS_CXX23

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

#else // ^^^ _HAS_CXX23 / !_HAS_CXX23 vvv

void test_flat_set() {}

#endif // ^^^ !_HAS_CXX23 ^^^
