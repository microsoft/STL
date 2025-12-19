// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <version>
#if defined(__cpp_lib_flat_set ) && _HAS_CXX23

#include <flat_set>

#include "shared_test.hpp"

void test_flat_set() {
    std::flat_set container{1, 2, 3};
    shared_test(container);

    std::flat_multiset container2{1, 2, 3};
    shared_test(container2);
}

#else // ^^^ defined(__cpp_lib_flat_set) && _HAS_CXX23 / feature not available vvv

void test_flat_set() {}

#endif // feature not available
