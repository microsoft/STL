// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <valarray>

#include "shared_test.hpp"

void test_valarray() {
#if 0 // TRANSITION, fails at the moment, fixed by not-yet accepted GH-5847
    std::valarray<int> container{1, 2, 3};
    shared_test(container);
#endif
}
