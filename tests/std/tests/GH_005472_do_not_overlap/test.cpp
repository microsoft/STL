// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// env.lst defines _MSVC_STL_HARDENING to 1.

#include <algorithm>

#include <test_death.hpp>

using namespace std;

int arr[3] = {};

int main(int argc, char* argv[]) {
#if _ITERATOR_DEBUG_LEVEL == 2
    std_testing::death_test_executive exec;

    exec.add_death_tests({
        +[] { swap_ranges(arr, arr + 2, arr + 1); },
        +[] { copy_if(arr, arr + 2, arr + 1, [](auto) { return false; }); },
        +[] { reverse_copy(arr, arr + 2, arr + 1); },
        +[] { replace_copy(arr, arr + 2, arr + 1, 1, 2); },
        +[] { replace_copy_if(arr, arr + 2, arr + 1, [](auto) { return false; }, 2); },
        +[] { rotate_copy(arr, arr + 1, arr + 2, arr + 1); },
        +[] { remove_copy(arr, arr + 2, arr + 1, 1); },
        +[] { remove_copy_if(arr, arr + 2, arr + 1, [](auto) { return false; }); },
        +[] { unique_copy(arr, arr + 2, arr + 1); },
#if _HAS_CXX20
        +[] { ranges::copy_if(arr, arr + 2, arr + 1, [](auto) { return false; }); },
        +[] { ranges::reverse_copy(arr, arr + 2, arr + 1); },
        +[] { ranges::replace_copy(arr, arr + 2, arr + 1, 1, 2); },
        +[] { ranges::replace_copy_if(arr, arr + 2, arr + 1, [](auto) { return false; }, 2); },
        +[] { ranges::rotate_copy(arr, arr + 1, arr + 2, arr + 1); },
        +[] { ranges::remove_copy(arr, arr + 2, arr + 1, 1); },
        +[] { ranges::remove_copy_if(arr, arr + 2, arr + 1, [](auto) { return false; }); },
        +[] { ranges::unique_copy(arr, arr + 2, arr + 1); },
#endif // _HAS_CXX20
    });

    return exec.run(argc, argv);
#else // ^^^ _ITERATOR_DEBUG_LEVEL == 2 / _ITERATOR_DEBUG_LEVEL != 2 vvv
    (void) argc;
    (void) argv;
    return 0; // This test is only for iterator debug mode
#endif // ^^^ _ITERATOR_DEBUG_LEVEL != 2 ^^^
}
