// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <execution>
#include <forward_list>
#include <iterator>
#include <list>
#include <numeric>
#include <random>
#include <string_view>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

const auto my_equal_to = [](auto a, auto b) { return a == b; };

template <template <class...> class HaystackContainer>
void test_case_search_n_parallel(const size_t testSize, mt19937& gen) {
    HaystackContainer<int> haystack(testSize);
    if (testSize == 0) {
        assert(haystack.end() == search_n(par, haystack.begin(), haystack.end(), 1, 1.0));
        assert(haystack.end() == search_n(par, haystack.begin(), haystack.end(), 1, 1.0, my_equal_to));
        return;
    }

    uniform_int_distribution<size_t> startDist(0, testSize - 1);
    size_t startAt = startDist(gen);
    uniform_int_distribution<size_t> sizeDist(1, testSize - startAt);
    size_t expectedSize = sizeDist(gen);
    auto expected       = next(haystack.begin(), static_cast<ptrdiff_t>(startAt));
    auto expectedEnd    = next(expected, static_cast<ptrdiff_t>(expectedSize));
    fill(expected, expectedEnd, 1729);
    assert(expected == search_n(par, haystack.begin(), haystack.end(), static_cast<int>(expectedSize), 1729));
    assert(
        expected == search_n(par, haystack.begin(), haystack.end(), static_cast<int>(expectedSize), 1729, my_equal_to));
}

template <template <class...> class HaystackContainer>
void test_case_runs_search_n_parallel(const int runCount) {
    // same as Sum[n = 1, n -> runCount, n + 1]
    // == ((runCount + 1)(runCount + 2)/2) - 1
    // == (runCount^2 + 3 * runCount + 3) / 2 - 1
    // == (runCount^2 + 3 * runCount + 1) / 2
    int totalLength = (runCount * runCount + 3 * runCount + 1) / 2;
    HaystackContainer<char> haystack(static_cast<size_t>(totalLength));
    vector<typename HaystackContainer<char>::iterator> expectedResults;

    // fill haystack with the pattern F T F T T F T T T F T T T T ...
    // and expected results             ^   ^     ^       ^
    auto b = haystack.begin();
    for (int range = 1; range <= runCount; ++range) {
        *b = 'F';
        ++b;
        expectedResults.push_back(b);
        b = fill_n(b, range, 'T');
    }

    // check that each of the repeating sub-patterns returns the right result
    for (int idx = 1; idx <= runCount; ++idx) {
        auto expected = expectedResults[static_cast<size_t>(idx - 1)];
        auto actual   = search_n(par, haystack.begin(), haystack.end(), idx, 'T');
        assert(expected == actual);
    }
}

int main() {
    { // test that an empty candidate set results in match at the beginning
        auto haystack = "cute fluffy kittens"sv;
        assert(haystack.begin() == search_n(par, haystack.begin(), haystack.end(), 0, 'c'));
        assert(haystack.begin()
               == search_n(par, haystack.begin(), haystack.end(), 0, 'c', [](auto, auto) -> bool { abort(); }));
    }

    { // test that the first match is found
        auto haystack = "see cute cuddly cats"sv;
        assert(haystack.begin() + 4 == search_n(par, haystack.begin(), haystack.end(), 1, 'c'));
        assert(haystack.begin() + 4 == search_n(par, haystack.begin(), haystack.end(), 1, 'c', my_equal_to));
    }

    { // test that the whole haystack is used
        auto haystack = "the cats are cccccccute and cuddly"sv;
        assert(haystack.begin() + 13 == search_n(par, haystack.begin(), haystack.end(), 7, 'c'));
        assert(haystack.begin() + 13 == search_n(par, haystack.begin(), haystack.end(), 7, 'c', my_equal_to));
    }

    { // test match of whole input
        auto haystack = "cccc"sv;
        assert(haystack.begin() == search_n(par, haystack.begin(), haystack.end(), 4, 'c'));
        assert(haystack.begin() == search_n(par, haystack.begin(), haystack.end(), 4, 'c', my_equal_to));
    }

    mt19937 gen(1729);

    parallel_test_case(test_case_search_n_parallel<forward_list>, gen);
    parallel_test_case(test_case_search_n_parallel<list>, gen);
    parallel_test_case(test_case_search_n_parallel<vector>, gen);

    test_case_runs_search_n_parallel<forward_list>(100);
    test_case_runs_search_n_parallel<list>(100);
    test_case_runs_search_n_parallel<vector>(100);
}
