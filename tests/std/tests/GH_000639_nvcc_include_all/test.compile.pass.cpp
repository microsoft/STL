// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#define _MSVC_TESTING_NVCC

#include <__msvc_all_public_headers.hpp>

using namespace std;

#if _HAS_CXX20
// Test VSO-2411436 "[Feedback] CUDA (12.8) host code compilation fails with std::format and VS2022"
void test_VSO_2411436() {
    (void) format("{}", 1729);
}
#endif // _HAS_CXX20

void test_removed_workaround_for_iterator_traits() {
    using Cat = iterator_traits<vector<int>::iterator>::iterator_category;
    static_assert(is_same_v<Cat, random_access_iterator_tag>, "Expected vector::iterator to be random-access");
}

void test_removed_workaround_for_static_call_operators() {
    (void) hash<double>{}(3.14);
}

void test_removed_workaround_for_intrinsics() {
    (void) ceil(3.14f);

#if _HAS_CXX20
    (void) countl_zero(1729u);
    (void) countr_zero(1729u);
    (void) popcount(1729u);

    const auto vw   = views::iota(0ll, 10ll);
    const auto dist = vw.end() - vw.begin();
    (void) (dist / dist);
#endif // _HAS_CXX20
}

void test_removed_workaround_for_nocheck_type_traits() {
    const pair<int, int> src{11, 22};
    pair<int, int> dst{33, 44};
    dst = src;
}

#if _HAS_CXX20
void test_removed_workaround_for_tzdb_list() {
    (void) chrono::get_tzdb_list();
}
#endif // _HAS_CXX20
