// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <execution>
#include <functional>
#include <numeric>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

void test_case_is_heap_parallel(const size_t testSize) {
    vector<size_t> v(testSize, 0UL);

    assert(is_heap(par, v.begin(), v.end()));
    assert(is_heap(par, v.begin(), v.end(), greater()));
    assert(is_heap_until(par, v.begin(), v.end()) == v.end());
    assert(is_heap_until(par, v.begin(), v.end(), greater()) == v.end());

    if (testSize < 2) {
        return;
    }

    // [0,...,0,1,0,...,0], with the 1 placed at each index
    // index = 0
    v[0] = 1;
    assert(is_heap(par, v.begin(), v.end()));
    assert(!is_heap(par, v.begin(), v.end(), greater()));
    assert(is_heap_until(par, v.begin(), v.end()) == v.end());
    assert(is_heap_until(par, v.begin(), v.end(), greater()) == v.begin() + 1);
    v[0] = 0;

    // (0, testSize - 1]
    const auto limit = std::min(testSize, quadratic_complexity_case_limit);
    for (auto i = 1U; i < limit; ++i) {
        v[i] = 1;
        assert(!is_heap(par, v.begin(), v.end()));
        assert(is_heap_until(par, v.begin(), v.end()) == v.begin() + static_cast<int>(i));
        if (i < testSize / 2) {
            assert(!is_heap(par, v.begin(), v.end(), greater()));
            assert(is_heap_until(par, v.begin(), v.end(), greater()) == v.begin() + (static_cast<int>(i) * 2) + 1);
        } else {
            assert(is_heap(par, v.begin(), v.end(), greater()));
            assert(is_heap_until(par, v.begin(), v.end(), greater()) == v.end());
        }
        v[i] = 0;
    }

    // all test cases exhausted for testSize == 2 at this point
    if (testSize == 2) {
        return;
    }

    // increasing list starting at 1
    iota(v.begin(), v.end(), 1UL);
    assert(!is_heap(par, v.begin(), v.end()));
    assert(is_heap_until(par, v.begin(), v.end()) == v.begin() + 1);
    assert(is_heap(par, v.begin(), v.end(), greater()));
    assert(is_heap_until(par, v.begin(), v.end(), greater()) == v.end());

    // place 0 at each index
    // index = 0
    const auto secondElement = v.begin() + 1;
    v[0]                     = 0;
    assert(!is_heap(par, v.begin(), v.end()));
    assert(is_heap_until(par, v.begin(), v.end()) == secondElement);
    assert(is_heap(par, v.begin(), v.end(), greater()));
    assert(is_heap_until(par, v.begin(), v.end(), greater()) == v.end());
    v[0] = 1;

    // index = 1
    v[1] = 0;
    assert(!is_heap(par, v.begin(), v.end()));
    assert(is_heap_until(par, v.begin(), v.end()) == secondElement + 1);
    assert(!is_heap(par, v.begin(), v.end(), greater()));
    assert(is_heap_until(par, v.begin(), v.end(), greater()) == secondElement);
    v[1] = 2;

    for (auto i = 2U; i < limit; ++i) {
        const auto old = v[i];
        v[i]           = 0;
        assert(!is_heap(par, v.begin(), v.end()));
        assert(is_heap_until(par, v.begin(), v.end()) == secondElement);
        assert(!is_heap(par, v.begin(), v.end(), greater()));
        assert(is_heap_until(par, v.begin(), v.end(), greater()) == v.begin() + static_cast<int>(i));
        v[i] = old;
    }

    // decreasing list starting at testSize
    iota(v.rbegin(), v.rend(), 1U);
    assert(is_heap(par, v.begin(), v.end()));
    assert(is_heap_until(par, v.begin(), v.end()) == v.end());
    assert(!is_heap(par, v.begin(), v.end(), greater()));
    assert(is_heap_until(par, v.begin(), v.end(), greater()) == secondElement);

    // place 0 at each index
    // index = 0
    v[0] = 0;
    assert(!is_heap(par, v.begin(), v.end()));
    assert(is_heap_until(par, v.begin(), v.end()) == secondElement);
    assert(!is_heap(par, v.begin(), v.end(), greater()));
    assert(is_heap_until(par, v.begin(), v.end(), greater()) == v.begin() + 3);
    v[0] = testSize;

    for (auto i = 1U; i < limit; ++i) {
        const auto old = v[i];
        v[i]           = 0;
        if (i < testSize / 2) {
            assert(!is_heap(par, v.begin(), v.end()));
            assert(is_heap_until(par, v.begin(), v.end()) == v.begin() + (static_cast<int>(i) * 2) + 1);
        } else {
            assert(is_heap(par, v.begin(), v.end()));
            assert(is_heap_until(par, v.begin(), v.end()) == v.end());
        }
        assert(!is_heap(par, v.begin(), v.end(), greater()));
        assert(is_heap_until(par, v.begin(), v.end(), greater()) == secondElement);
        v[i] = old;
    }
}

int main() {
    parallel_test_case(test_case_is_heap_parallel);
}
