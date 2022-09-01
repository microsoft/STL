// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _CORE_HEADERS_ONLY
#define _ENFORCE_ONLY_CORE_HEADERS // Also test GH-311 "Make it easy to limit the STL to core features"

#include <__msvc_all_public_headers.hpp>

#ifdef _YVALS
#error Core headers should not include <yvals.h>.
#endif

void test_gh_2699() {
    // GH-2699 Vectorized swap emits error LNK2019:
    // unresolved external symbol ___std_swap_ranges_trivially_swappable_noalias
    int cats[]{10, 20, 30};
    int dogs[]{40, 50, 60};

    std::swap(cats, dogs);
    assert(cats[1] == 50);
    assert(dogs[1] == 20);

#ifdef __cpp_lib_ranges
    std::ranges::swap(cats, dogs);
    assert(cats[1] == 20);
    assert(dogs[1] == 50);
#endif // __cpp_lib_ranges
}

int main() {
    test_gh_2699();
}
