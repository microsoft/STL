// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <execution>
#include <forward_list>
#include <list>
#include <numeric>
#include <string_view>
#include <vector>

#include <parallel_algorithms_utilities.hpp>

using namespace std;
using namespace std::execution;

const auto my_equal_to = [](auto a, auto b) { return a == b; };

template <template <class...> class NeedleContainer, template <class...> class CandidateContainer>
void test_case_find_first_of_parallel(const size_t testSize) {
    NeedleContainer<int> tmp(testSize);
    iota(tmp.begin(), tmp.end(), 0);
    CandidateContainer<double> candidates(testSize);
    iota(candidates.begin(), candidates.end(), 0);
    auto expected        = tmp.begin();
    auto candidatesBegin = candidates.begin();
    const auto limit     = std::min(testSize, quadratic_complexity_case_limit);
    for (size_t idx = 0; idx < limit; ++idx) {
        assert(expected == find_first_of(par, tmp.begin(), tmp.end(), candidatesBegin, candidates.end()));
        ++candidatesBegin;
        ++expected;
    }
}

int main() {
    { // test that an empty candidate set results in no match
        auto needle  = "cute fluffy kittens"sv;
        auto targets = ""sv;
        assert(needle.end() == find_first_of(par, needle.begin(), needle.end(), targets.begin(), targets.end()));
        assert(needle.end()
               == find_first_of(par, needle.begin(), needle.end(), targets.begin(), targets.end(),
                   [](auto, auto) -> bool { abort(); }));
    }

    { // test that the first match is found
        auto needle  = "see cute cuddly cats"sv;
        auto targets = "c"sv;
        assert(needle.begin() + 4 == find_first_of(par, needle.begin(), needle.end(), targets.begin(), targets.end()));
        assert(needle.begin() + 4
               == find_first_of(par, needle.begin(), needle.end(), targets.begin(), targets.end(), my_equal_to));
    }

    { // test that the first target is found
        auto needle  = "see cute cuddly cats"sv;
        auto targets = "ce"sv;
        assert(needle.begin() + 1 == find_first_of(par, needle.begin(), needle.end(), targets.begin(), targets.end()));
        assert(needle.begin() + 1
               == find_first_of(par, needle.begin(), needle.end(), targets.begin(), targets.end(), my_equal_to));
    }

    parallel_test_case(test_case_find_first_of_parallel<forward_list, forward_list>);
    parallel_test_case(test_case_find_first_of_parallel<list, list>);
    parallel_test_case(test_case_find_first_of_parallel<list, vector>);
    parallel_test_case(test_case_find_first_of_parallel<vector, forward_list>);
    parallel_test_case(test_case_find_first_of_parallel<vector, vector>);
}
