// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <deque>
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
void test_case_is_sorted_parallel(const size_t testSize) {
    Container<size_t> c(testSize, 0UL);

    assert(is_sorted(par, c.begin(), c.end()));
    assert(is_sorted(par, c.begin(), c.end(), greater()));
    assert(is_sorted_until(par, c.begin(), c.end()) == c.end());
    assert(is_sorted_until(par, c.begin(), c.end(), greater()) == c.end());

    if (testSize < 2) {
        return;
    }

    // [0,...,0,1,0,...,0], with the 1 placed at each index
    auto next       = c.begin();
    const auto last = c.end();

    // first index:
    *next = 1;
    assert(!is_sorted(par, c.begin(), c.end()));
    assert(is_sorted(par, c.begin(), c.end(), greater()));
    assert(is_sorted_until(par, c.begin(), c.end()) == std::next(c.begin()));
    assert(is_sorted_until(par, c.begin(), c.end(), greater()) == c.end());
    *next = 0;

    // (first index, last index)
    auto remainingAttempts = quadratic_complexity_case_limit;
    ++next;
    {
        size_t i = 0;
        for (; i < testSize - 2 && remainingAttempts != 0; ++i, --remainingAttempts) {
            *next = 1;
            assert(!is_sorted(par, c.begin(), c.end()));
            assert(!is_sorted(par, c.begin(), c.end(), greater()));
            assert(is_sorted_until(par, c.begin(), c.end()) == std::next(next));
            assert(is_sorted_until(par, c.begin(), c.end(), greater()) == next);
            *next = 0;
            ++next;
        }

        std::advance(next, static_cast<ptrdiff_t>(testSize - 2 - i));
    }

    // last index:
    *next = 1;
    assert(is_sorted(par, c.begin(), c.end()));
    assert(!is_sorted(par, c.begin(), c.end(), greater()));
    assert(is_sorted_until(par, c.begin(), c.end()) == c.end());
    assert(is_sorted_until(par, c.begin(), c.end(), greater()) == next);

    // all test cases covered for this size above:
    // * first < second
    // * first == second
    // * first > second
    if (testSize == 2) {
        return;
    }

    // increasing list starting at 1
    iota(c.begin(), c.end(), 1UL);
    assert(is_sorted(par, c.begin(), c.end()));
    assert(!is_sorted(par, c.begin(), c.end(), greater()));
    assert(is_sorted_until(par, c.begin(), c.end()) == c.end());
    assert(is_sorted_until(par, c.begin(), c.end(), greater()) == std::next(c.begin()));

    // breaking the increasing list at each index [1, 2, ..., 0, ..., testSize-1, testSize]
    // Note that we skip the first case since [0, 2, ..., testSize-1, testSize] is still sorted
    remainingAttempts        = quadratic_complexity_case_limit;
    int i                    = 1;
    const auto secondElement = std::next(c.begin());
    const auto thirdElement  = std::next(secondElement);
    for (auto first = secondElement; first != last; ++first) {
        const auto old = *first;
        *first         = 0;
        assert(!is_sorted(par, c.begin(), c.end()));
        assert(!is_sorted(par, c.begin(), c.end(), greater()));
        assert(is_sorted_until(par, c.begin(), c.end()) == first);
        if (i == 1) { // [1,0,3,...]
            assert(is_sorted_until(par, c.begin(), c.end(), greater()) == thirdElement);
        } else { // [1,2,...,0,...]
            assert(is_sorted_until(par, c.begin(), c.end(), greater()) == secondElement);
        }
        *first = old;
        ++i;
        if (--remainingAttempts == 0) {
            break;
        }
    }

    // increasing list except for first element
    if (remainingAttempts != 0) {
        *c.begin() = testSize;
        assert(!is_sorted(par, c.begin(), c.end()));
        assert(!is_sorted(par, c.begin(), c.end(), greater()));
        assert(is_sorted_until(par, c.begin(), c.end()) == std::next(c.begin()));
        assert(is_sorted_until(par, c.begin(), c.end(), greater()) == std::next(c.begin(), 2));
    }

    // decreasing list
    size_t val = testSize;
    for (auto& elem : c) {
        elem = val--;
    }
    assert(!is_sorted(par, c.begin(), c.end()));
    assert(is_sorted(par, c.begin(), c.end(), greater()));
    assert(is_sorted_until(par, c.begin(), c.end()) == std::next(c.begin()));
    assert(is_sorted_until(par, c.begin(), c.end(), greater()) == c.end());
}

int main() {
    parallel_test_case(test_case_is_sorted_parallel<vector>);
    parallel_test_case(test_case_is_sorted_parallel<forward_list>);
    parallel_test_case(test_case_is_sorted_parallel<list>);
    parallel_test_case(test_case_is_sorted_parallel<deque>);
}
