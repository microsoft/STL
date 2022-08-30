// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <vector>

#include <test_death.hpp>
using namespace std;

void test_case_swap_ranges_overlap_1() {
    int arr[8] = {10, 20, 30, 40, 50, 60, 70, 80};
    swap_ranges(arr + 1, arr + 4, arr + 3);
}

void test_case_swap_ranges_overlap_2() {
    int arr[8] = {10, 20, 30, 40, 50, 60, 70, 80};
    swap_ranges(arr + 4, arr + 7, arr + 2);
}

void test_case_vector_assign_front() {
    vector<int> vec{11, 22, 33};
    vec.assign(5, vec.front());
}

void test_case_vector_assign_mid() {
    vector<int> vec{11, 22, 33};
    vec.assign(5, vec[1]);
}

void test_case_vector_assign_back() {
    vector<int> vec{11, 22, 33};
    vec.assign(5, vec.back());
}

int main(int argc, char* argv[]) {
    std_testing::death_test_executive exec;

#if _ITERATOR_DEBUG_LEVEL == 2
    exec.add_death_tests({
        test_case_swap_ranges_overlap_1,
        test_case_swap_ranges_overlap_2,
        test_case_vector_assign_front,
        test_case_vector_assign_mid,
        test_case_vector_assign_back,
    });
#endif // _ITERATOR_DEBUG_LEVEL == 2

    return exec.run(argc, argv);
}
