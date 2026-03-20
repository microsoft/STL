// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// REQUIRES: x64

#ifdef _M_X64

#include <cstddef>
#include <vector>

#include <test_min_max_element_support.hpp>
#include <test_vector_algorithms_support.hpp>

using namespace std;

void test_gh_3617() {
    // Test GH-3617 "<algorithm>: Silent bad codegen for vectorized meow_element() above 4 GB".
    constexpr size_t n = 0x4000'0010;

    vector<int> v(n, 25);
    v[n - 2] = 24;
    v[n - 1] = 26;

    test_case_min_max_element(v);
}

int main() {
    run_tests_with_different_isa_levels([] { test_gh_3617(); });
}
#else // ^^^ x64 / other architectures vvv
int main() {}
#endif // ^^^ other architectures ^^^
