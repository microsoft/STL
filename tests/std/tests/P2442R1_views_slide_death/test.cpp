// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <ranges>

#include <test_death.hpp>

using namespace std;
using ranges::slide_view;

static constexpr int some_ints[] = {0, 1, 2, 3};

void test_view_negative_window_size() {
    slide_view(some_ints, -1); // window size must be positive
}

void test_view_zero_window_size() {
    slide_view(some_ints, 0); // window size must be positive
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL != 0
    exec.add_death_tests({
        test_view_negative_window_size,
        test_view_zero_window_size,
    });
#endif // _ITERATOR_DEBUG_LEVEL != 0

    return exec.run(argc, argv);
}
