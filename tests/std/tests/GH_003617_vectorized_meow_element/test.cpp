// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// REQUIRES: x64

#ifdef _M_X64

#include <cstddef>
#include <isa_availability.h>
#include <vector>

#include "test_min_max_element_support.hpp"

using namespace std;

extern "C" long __isa_enabled;

void disable_instructions(ISA_AVAILABILITY isa) {
    __isa_enabled &= ~(1UL << static_cast<unsigned long>(isa));
}

void test_gh_3617() {
    // Test GH-3617 "<algorithm>: Silent bad codegen for vectorized meow_element() above 4 GB".
    constexpr size_t n = 0x4000'0010;

    vector<int> v(n, 25);
    v[n - 2] = 24;
    v[n - 1] = 26;

    test_case_min_max_element(v);
}

int main() {
    test_gh_3617();

    disable_instructions(__ISA_AVAILABLE_AVX2);
    test_gh_3617();

    disable_instructions(__ISA_AVAILABLE_SSE42);
    test_gh_3617();
}
#else // ^^^ x64 / other architectures vvv
int main() {}
#endif // ^^^ other architectures ^^^
