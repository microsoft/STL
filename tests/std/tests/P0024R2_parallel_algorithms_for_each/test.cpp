// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <atomic>
#include <cassert>
#include <execution>
#include <forward_list>
#include <iterator>
#include <list>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

int g_forEachNCalls = 0;
struct for_each_n_tester {
    int calledCount = 0;

    for_each_n_tester()                                    = default;
    for_each_n_tester(const for_each_n_tester&)            = delete;
    for_each_n_tester(for_each_n_tester&&)                 = default;
    for_each_n_tester& operator=(const for_each_n_tester&) = delete;
    for_each_n_tester& operator=(for_each_n_tester&&)      = delete;

    void operator()(const int x) {
        assert(calledCount == g_forEachNCalls);
        assert(x == calledCount + 10);
        ++calledCount;
        ++g_forEachNCalls;
    }
};

void test_case_for_each_n() {
    const int arr[]      = {10, 11, 12, 13, 14, 15};
    const int* const ptr = arr;
    assert(for_each_n(arr, 0, for_each_n_tester{}) == ptr);
    assert(for_each_n(arr, size(arr), for_each_n_tester{}) == end(arr));
    g_forEachNCalls = 0;
    assert(for_each_n(ptr, 0, for_each_n_tester{}) == ptr);
    assert(for_each_n(ptr, size(arr), for_each_n_tester{}) == end(arr));
}

const auto call_only_once = [](atomic<bool>& b) { assert(!b.exchange(true)); };

const auto atomic_identity = [](atomic<bool>& b) { return b.load(); };

template <template <class...> class Container>
struct test_case_for_each_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        Container<atomic<bool>> c(testSize);
        for_each(exec, c.begin(), c.end(), call_only_once);
        assert(all_of(c.begin(), c.end(), atomic_identity));
    }
};

template <template <class...> class Container>
struct test_case_for_each_n_parallel {
    template <typename ExecutionPolicy>
    void operator()(const size_t testSize, const ExecutionPolicy& exec) {
        Container<atomic<bool>> c(testSize);
        auto result = for_each_n(exec, c.begin(), testSize, call_only_once);
        assert(result == c.end());
        assert(all_of(c.begin(), c.end(), atomic_identity));
    }
};

int main() {
    test_case_for_each_n();
    parallel_test_case(test_case_for_each_parallel<forward_list>{}, par);
    parallel_test_case(test_case_for_each_parallel<list>{}, par);
    parallel_test_case(test_case_for_each_parallel<vector>{}, par);
    parallel_test_case(test_case_for_each_n_parallel<forward_list>{}, par);
    parallel_test_case(test_case_for_each_n_parallel<list>{}, par);
    parallel_test_case(test_case_for_each_n_parallel<vector>{}, par);
#if _HAS_CXX20
    parallel_test_case(test_case_for_each_parallel<forward_list>{}, unseq);
    parallel_test_case(test_case_for_each_parallel<list>{}, unseq);
    parallel_test_case(test_case_for_each_parallel<vector>{}, unseq);
    parallel_test_case(test_case_for_each_n_parallel<forward_list>{}, unseq);
    parallel_test_case(test_case_for_each_n_parallel<list>{}, unseq);
    parallel_test_case(test_case_for_each_n_parallel<vector>{}, unseq);
#endif // _HAS_CXX20
}
