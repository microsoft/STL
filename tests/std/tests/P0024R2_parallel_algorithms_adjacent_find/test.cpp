// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <execution>
#include <forward_list>
#include <functional>
#include <iterator>
#include <list>
#include <numeric>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

template <template <class...> class Container>
void test_case_adjacent_find_parallel(const size_t testSize) {
    Container<int> tmp(testSize);
    iota(tmp.begin(), tmp.end(), 0);

    assert(adjacent_find(par, tmp.begin(), tmp.end()) == tmp.end());
    const auto less_result = adjacent_find(par, tmp.begin(), tmp.end(), less<>{});
    const auto ne_result   = adjacent_find(par, tmp.begin(), tmp.end(), not_equal_to<>{});
    if (testSize < 2) {
        assert(less_result == tmp.end());
        assert(ne_result == tmp.end());
        return;
    }

    assert(less_result == tmp.begin());
    assert(ne_result == tmp.begin());

    auto remaining_attempts = quadratic_complexity_case_limit;
    auto target             = tmp.begin();
    for (auto next = target; ++next != tmp.end(); target = next) {
        auto old = *target;
        *target  = *next;
        assert(adjacent_find(par, tmp.begin(), tmp.end()) == target);
        *target = old;
        if (--remaining_attempts == 0) {
            return;
        }
    }
}

int main() {
    parallel_test_case(test_case_adjacent_find_parallel<forward_list>);
    parallel_test_case(test_case_adjacent_find_parallel<list>);
    parallel_test_case(test_case_adjacent_find_parallel<vector>);
}
