// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if _HAS_CXX23

#include <flat_map>

#include "shared_test.hpp"

void test_flat_map() {
    {
        std::flat_map<int, int> container{
            {1, 5},
            {3, 7},
        };
        shared_test(container);
    }

    {
        std::flat_multimap<int, int> container2{
            {1, 5},
            {3, 7},
        };
        shared_test(container2);
    }
}

#else // ^^^ _HAS_CXX23 / !_HAS_CXX23 vvv

void test_flat_map() {}

#endif // ^^^ !_HAS_CXX23 ^^^
