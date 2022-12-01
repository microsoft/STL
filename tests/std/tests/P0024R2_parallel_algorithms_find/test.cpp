// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <execution>
#include <forward_list>
#include <iterator>
#include <list>
#include <numeric>
#include <string>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

const char zero{};
const char one     = '\x01';
const auto is_zero = [](char c) { return c == zero; };
const auto is_one  = [](char c) { return c == one; };

template <template <class...> class Container>
void test_case_find_parallel(const size_t testSize) {
    Container<char> tmp(testSize);

    assert(find(par, tmp.begin(), tmp.end(), zero) == tmp.begin());
    assert(find_if(par, tmp.begin(), tmp.end(), is_zero) == tmp.begin());
    assert(find_if_not(par, tmp.begin(), tmp.end(), is_one) == tmp.begin());
    assert(find(par, tmp.begin(), tmp.end(), one) == tmp.end());
    assert(find_if(par, tmp.begin(), tmp.end(), is_one) == tmp.end());
    assert(find_if_not(par, tmp.begin(), tmp.end(), is_zero) == tmp.end());

    {
        // testing every possible combo takes too long
#ifdef EXHAUSTIVE
        for (auto target = tmp.begin(); target != tmp.end(); ++target) {
            *target = one;
            assert(find(par, tmp.begin(), tmp.end(), one) == target);
            assert(find_if(par, tmp.begin(), tmp.end(), is_one) == target);
            assert(find_if_not(par, tmp.begin(), tmp.end(), is_zero) == target);
            *target = zero;
        }
#else // ^^^ EXHAUSTIVE / !EXHAUSTIVE vvv
        if (testSize != 0) {
            auto middle = tmp.begin();
            advance(middle, static_cast<ptrdiff_t>(testSize / 2));
            *middle = one;
            assert(find(par, tmp.begin(), tmp.end(), one) == middle);
            assert(find_if(par, tmp.begin(), tmp.end(), is_one) == middle);
            assert(find_if_not(par, tmp.begin(), tmp.end(), is_zero) == middle);
        }
#endif // EXHAUSTIVE
    }
}

void test_case_find_with_many_counterexamples_picks_first() {
    vector<unsigned int> target(1729);
    target[450]  = 8u;
    target[1000] = 8u;
    target[500]  = 8u;
    assert(find(par, target.begin(), target.end(), 8u) == target.begin() + 450);
}

int main() {
    parallel_test_case(test_case_find_parallel<forward_list>);
    parallel_test_case(test_case_find_parallel<list>);
    parallel_test_case(test_case_find_parallel<vector>);

    test_case_find_with_many_counterexamples_picks_first();
}
