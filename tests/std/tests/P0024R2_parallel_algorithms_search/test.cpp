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

template <template <class...> class HaystackContainer, template <class...> class NeedleContainer>
void test_case_search_parallel(const size_t testSize, mt19937& gen) {
    HaystackContainer<int> haystack(testSize);
    if (testSize == 0) {
        NeedleContainer<double> needle(6);
        iota(needle.begin(), needle.end(), 0);
        assert(haystack.end() == search(par, haystack.begin(), haystack.end(), needle.begin(), needle.end()));
        assert(
            haystack.end() == search(par, haystack.begin(), haystack.end(), needle.begin(), needle.end(), my_equal_to));
        return;
    }

    uniform_int_distribution<size_t> startDist(0, testSize - 1);
    size_t startAt = startDist(gen);
    uniform_int_distribution<size_t> sizeDist(1, testSize - startAt);
    size_t expectedSize = sizeDist(gen);
    auto expected       = next(haystack.begin(), static_cast<ptrdiff_t>(startAt));
    auto expectedEnd    = next(expected, static_cast<ptrdiff_t>(expectedSize));
    iota(expected, expectedEnd, 1);
    NeedleContainer<double> needle(expectedSize);
    iota(needle.begin(), needle.end(), 1);
    assert(expected == search(par, haystack.begin(), haystack.end(), needle.begin(), needle.end()));
    assert(expected == search(par, haystack.begin(), haystack.end(), needle.begin(), needle.end(), my_equal_to));
}

template <template <class...> class HaystackContainer, template <class...> class NeedleContainer>
void test_case_suffixes_search_parallel(const int suffixCount) {
    NeedleContainer<double> needle(static_cast<size_t>(suffixCount));
    iota(needle.begin(), needle.end(), 0);

    int totalLength = (suffixCount * (suffixCount + 1)) / 2;
    HaystackContainer<int> haystack(static_cast<size_t>(totalLength));
    vector<typename HaystackContainer<int>::iterator> expectedResults;

    // fill haystack with the pattern 0 0 1 0 1 2 0 1 2 3 0 1 2 3 4 [...]
    // and expected results:          ^ ^   ^     ^       ^
    auto b = haystack.begin();
    for (int range = 0; range < suffixCount; ++range) {
        expectedResults.push_back(b);
        auto newB = next(b, range + 1);
        iota(b, newB, 0);
        b = newB;
    }

    // check that each of the repeating sub-patterns returns the right result
    for (int idx = 0; idx < suffixCount; ++idx) {
        auto expected = expectedResults[static_cast<size_t>(idx)];
        auto actual   = search(par, haystack.begin(), haystack.end(), needle.begin(), next(needle.begin(), idx + 1));
        assert(expected == actual);
    }
}

int main() {
    { // test that an empty candidate set results in match at the beginning
        auto haystack = "cute fluffy kittens"sv;
        auto needle   = ""sv;
        assert(haystack.begin() == search(par, haystack.begin(), haystack.end(), needle.begin(), needle.end()));
        assert(haystack.begin()
               == search(par, haystack.begin(), haystack.end(), needle.begin(), needle.end(),
                   [](auto, auto) -> bool { abort(); }));
    }

    { // test that the first match is found
        auto haystack = "see cute cuddly cats"sv;
        auto needle   = "c"sv;
        assert(haystack.begin() + 4 == search(par, haystack.begin(), haystack.end(), needle.begin(), needle.end()));
        assert(haystack.begin() + 4
               == search(par, haystack.begin(), haystack.end(), needle.begin(), needle.end(), my_equal_to));
    }

    { // test that the whole haystack is used
        auto haystack = "the cats are cute and cuddly"sv;
        auto needle   = "cu"sv;
        assert(haystack.begin() + 13 == search(par, haystack.begin(), haystack.end(), needle.begin(), needle.end()));
        assert(haystack.begin() + 13
               == search(par, haystack.begin(), haystack.end(), needle.begin(), needle.end(), my_equal_to));
    }

    { // test whole input match
        auto haystack = "the cats are cute and cuddly"sv;
        assert(haystack.begin() == search(par, haystack.begin(), haystack.end(), haystack.begin(), haystack.end()));
        assert(haystack.begin()
               == search(par, haystack.begin(), haystack.end(), haystack.begin(), haystack.end(), my_equal_to));
    }

    mt19937 gen(1729);

    parallel_test_case(test_case_search_parallel<forward_list, forward_list>, gen);
    parallel_test_case(test_case_search_parallel<list, list>, gen);
    parallel_test_case(test_case_search_parallel<list, vector>, gen);
    parallel_test_case(test_case_search_parallel<vector, forward_list>, gen);
    parallel_test_case(test_case_search_parallel<vector, vector>, gen);

    test_case_suffixes_search_parallel<forward_list, forward_list>(50);
    test_case_suffixes_search_parallel<list, list>(50);
    test_case_suffixes_search_parallel<list, vector>(50);
    test_case_suffixes_search_parallel<vector, forward_list>(50);
    test_case_suffixes_search_parallel<vector, vector>(50);
}
