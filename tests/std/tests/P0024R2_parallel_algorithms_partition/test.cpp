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
#include <random>
#include <string>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

template <class Container, class Iter, class Fn>
void assert_is_partition(Container& c, Iter first, Iter last, Iter div, Fn fn) {
    assert(all_of(first, div, fn));
    assert(none_of(div, last, fn));
    const auto expected = partition(c.begin(), c.end(), fn);
    assert(is_permutation(c.begin(), expected, first, div));
    assert(is_permutation(expected, c.end(), div, last));
}

const auto is_even = [](unsigned int i) { return (i & 0x1u) == 0; };

template <template <class...> class Container>
void test_case_partition_parallel(const size_t testSize, mt19937& gen) {
    Container<unsigned int> tmp(testSize);
    iota(tmp.begin(), tmp.end(), 0U);

    auto tmpStart = tmp;
    {
        assert(tmp.end() == partition(par, tmp.begin(), tmp.end(), [](auto) { return true; }));
        assert(tmp == tmpStart); // technically not guaranteed by the standard, but we want to provide this
        assert(tmp.begin() == partition(par, tmp.begin(), tmp.end(), [](auto) { return false; }));
        assert(tmp == tmpStart); // ditto technically not guaranteed

        const auto result         = partition(par, tmp.begin(), tmp.end(), is_even);
        const auto expectedFalses = testSize >> 1;
        const auto expectedTrues  = testSize - expectedFalses;
        assert(distance(tmp.begin(), result) == static_cast<ptrdiff_t>(expectedTrues));
        assert(distance(result, tmp.end()) == static_cast<ptrdiff_t>(expectedFalses));
        assert_is_partition(tmpStart, tmp.begin(), tmp.end(), result, is_even);
    }

    // "fuzz" testing:
    for (int i = 0; i < 100; ++i) {
        generate(tmp.begin(), tmp.end(), ref(gen));
        tmpStart          = tmp;
        const auto actual = partition(par, tmp.begin(), tmp.end(), is_even);
        assert_is_partition(tmpStart, tmp.begin(), tmp.end(), actual, is_even);
    }
}

int main() {
    mt19937 gen(1729);
    parallel_test_case(test_case_partition_parallel<forward_list>, gen);
    parallel_test_case(test_case_partition_parallel<list>, gen);
    parallel_test_case(test_case_partition_parallel<vector>, gen);
}
