// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if defined(__cpp_lib_flat_map)
static_assert(false, "When this feature is implemented, update this to a Standard mode check.");

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

#else // ^^^ defined(__cpp_lib_flat_map) / !defined(__cpp_lib_flat_map) vvv

void test_flat_map() {}

#endif // ^^^ !defined(__cpp_lib_flat_map) ^^^
