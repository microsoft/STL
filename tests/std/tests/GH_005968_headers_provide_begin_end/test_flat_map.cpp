// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if defined(__cpp_lib_flat_map)

#include <flat_map>

#include "shared_test.hpp"

void test_flat_map() {
    std::flat_map<int, int> container{
        {1, 5},
        {3, 7},
    };
    shared_test(container);

    std::flat_multimap<int, int> container2{
        {1, 5},
        {3, 7},
    };
    shared_test(container2);
}

#else // ^^^ defined(__cpp_lib_flat_map) / feature not available vvv

void test_flat_map() {}

#endif // feature not available
