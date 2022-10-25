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
void test_case_find_end_parallel(const size_t testSize, mt19937& gen) {
    HaystackContainer<int> haystack(testSize);
    if (testSize == 0) {
        NeedleContainer<double> needle(6);
        iota(needle.begin(), needle.end(), 0);
        assert(haystack.end() == find_end(par, haystack.begin(), haystack.end(), needle.begin(), needle.end()));
        assert(haystack.end()
               == find_end(par, haystack.begin(), haystack.end(), needle.begin(), needle.end(), my_equal_to));
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
    assert(expected == find_end(par, haystack.begin(), haystack.end(), needle.begin(), needle.end()));
    assert(expected == find_end(par, haystack.begin(), haystack.end(), needle.begin(), needle.end(), my_equal_to));
}

template <template <class...> class HaystackContainer, template <class...> class NeedleContainer>
void test_case_prefixes_find_end_parallel(const int prefixCount) {
    // fill needle with the pattern 4 3 2 1 0
    NeedleContainer<double> needle(static_cast<size_t>(prefixCount));
    auto needleBegin = needle.begin();
    for (int idx = 0; idx < prefixCount; ++idx) {
        *needleBegin = prefixCount - idx - 1;
        ++needleBegin;
    }

    int totalLength = (prefixCount * (prefixCount + 1)) / 2;
    HaystackContainer<int> haystack(static_cast<size_t>(totalLength));
    vector<typename HaystackContainer<int>::iterator> expectedResults;

    // fill haystack with the pattern [...] 4 3 2 1 0 3 2 1 0 2 1 0 1 0
    // and expected results:                ^         ^       ^     ^ ^
    auto b = haystack.begin();
    for (int range = 0; range < prefixCount; ++range) {
        expectedResults.push_back(b);
        for (int val = prefixCount - range; val--;) {
            *b = val;
            ++b;
        }
    }

    expectedResults.push_back(b);

    // check that each of the repeating sub-patterns returns the right result
    for (int idx = 0; idx <= prefixCount; ++idx) {
        auto expected = expectedResults[static_cast<size_t>(prefixCount - idx)];
        auto actual =
            find_end(par, haystack.begin(), haystack.end(), next(needle.begin(), prefixCount - idx), needle.end());
        assert(expected == actual);
    }
}

int main() {
    { // test that an empty candidate set results in no match
        auto haystack = "cute fluffy kittens"sv;
        auto needle   = ""sv;
        assert(haystack.end() == find_end(par, haystack.begin(), haystack.end(), needle.begin(), needle.end()));
        assert(haystack.end()
               == find_end(par, haystack.begin(), haystack.end(), needle.begin(), needle.end(),
                   [](auto, auto) -> bool { abort(); }));
    }

    { // test that the last match is found
        auto haystack = "see cute cuddly cats"sv;
        auto needle   = "c"sv;
        assert(haystack.begin() + 16 == find_end(par, haystack.begin(), haystack.end(), needle.begin(), needle.end()));
        assert(haystack.begin() + 16
               == find_end(par, haystack.begin(), haystack.end(), needle.begin(), needle.end(), my_equal_to));
    }

    { // test that the whole haystack is used
        auto haystack = "see cute cuddly cats"sv;
        auto needle   = "cu"sv;
        assert(haystack.begin() + 9 == find_end(par, haystack.begin(), haystack.end(), needle.begin(), needle.end()));
        assert(haystack.begin() + 9
               == find_end(par, haystack.begin(), haystack.end(), needle.begin(), needle.end(), my_equal_to));
    }

    { // test whole input match
        auto haystack = "the cats are cute and cuddly"sv;
        assert(haystack.begin() == find_end(par, haystack.begin(), haystack.end(), haystack.begin(), haystack.end()));
        assert(haystack.begin()
               == find_end(par, haystack.begin(), haystack.end(), haystack.begin(), haystack.end(), my_equal_to));
    }

    mt19937 gen(1729);

    parallel_test_case(test_case_find_end_parallel<forward_list, forward_list>, gen);
    parallel_test_case(test_case_find_end_parallel<list, list>, gen);
    parallel_test_case(test_case_find_end_parallel<list, vector>, gen);
    parallel_test_case(test_case_find_end_parallel<vector, forward_list>, gen);
    parallel_test_case(test_case_find_end_parallel<vector, vector>, gen);

    test_case_prefixes_find_end_parallel<forward_list, forward_list>(50);
    test_case_prefixes_find_end_parallel<list, list>(50);
    test_case_prefixes_find_end_parallel<list, vector>(50);
    test_case_prefixes_find_end_parallel<vector, forward_list>(50);
    test_case_prefixes_find_end_parallel<vector, vector>(50);
}
