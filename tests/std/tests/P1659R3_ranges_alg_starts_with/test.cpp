// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>
using namespace std;

#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::dangling, ranges::equal_to, ranges::iterator_t, ranges::starts_with;

    constexpr array<pair<int, int>, 3> haystack       = {{{0, 42}, {2, 42}, {4, 42}}};
    constexpr array<pair<int, int>, 1> short_haystack = {{{0, 42}}};
    constexpr array<pair<long, long>, 2> needle       = {{{13, 0}, {13, 2}}};
    constexpr array<pair<long, long>, 2> wrong_needle = {{{13, 0}, {13, 3}}};

    { // Validate sized ranges
        const same_as<bool> auto match = starts_with(haystack, needle, equal_to{}, get_first, get_second);
        assert(match);

        const same_as<bool> auto no_match = starts_with(haystack, wrong_needle, equal_to{}, get_first, get_second);
        assert(!no_match);

        const same_as<bool> auto no_match2 = starts_with(short_haystack, needle, equal_to{}, get_first, get_second);
        assert(!no_match2);
    }
    { // Validate non-sized ranges
        const same_as<bool> auto match = starts_with(
            basic_borrowed_range{haystack}, basic_borrowed_range{needle}, equal_to{}, get_first, get_second);
        assert(match);

        const same_as<bool> auto no_match = starts_with(
            basic_borrowed_range{haystack}, basic_borrowed_range{wrong_needle}, equal_to{}, get_first, get_second);
        assert(!no_match);

        const same_as<bool> auto no_match2 = starts_with(
            basic_borrowed_range{short_haystack}, basic_borrowed_range{needle}, equal_to{}, get_first, get_second);
        assert(!no_match2);
    }
    { // Validate sized iterator + sentinel pairs
        const same_as<bool> auto match = starts_with(
            haystack.begin(), haystack.end(), needle.begin(), needle.end(), equal_to{}, get_first, get_second);
        assert(match);

        const same_as<bool> auto no_match = starts_with(haystack.begin(), haystack.end(), wrong_needle.begin(),
            wrong_needle.end(), equal_to{}, get_first, get_second);
        assert(!no_match);

        const same_as<bool> auto no_match2 = starts_with(short_haystack.begin(), short_haystack.end(), needle.begin(),
            needle.end(), equal_to{}, get_first, get_second);
        assert(!no_match2);
    }
    { // Validate non-sized iterator + sentinel pairs
        basic_borrowed_range wrapped_haystack{haystack};
        basic_borrowed_range wrapped_needle{needle};
        const same_as<bool> auto match = starts_with(wrapped_haystack.begin(), wrapped_haystack.end(),
            wrapped_needle.begin(), wrapped_needle.end(), equal_to{}, get_first, get_second);
        assert(match);

        basic_borrowed_range wrapped_haystack2{haystack};
        basic_borrowed_range wrapped_wrong_needle{wrong_needle};
        const same_as<bool> auto no_match = starts_with(wrapped_haystack2.begin(), wrapped_haystack2.end(),
            wrapped_wrong_needle.begin(), wrapped_wrong_needle.end(), equal_to{}, get_first, get_second);
        assert(!no_match);

        basic_borrowed_range wrapped_short_haystack{short_haystack};
        basic_borrowed_range wrapped_needle2{needle};
        const same_as<bool> auto no_match2 = starts_with(wrapped_short_haystack.begin(), wrapped_short_haystack.end(),
            wrapped_needle2.begin(), wrapped_needle2.end(), equal_to{}, get_first, get_second);
        assert(!no_match2);
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

#ifndef _PREFAST_ // TRANSITION, GH-1030
struct instantiator {
    template <class In1, class In2>
    static void call(In1&& in1 = {}, In2&& in2 = {}) {
        using ranges::begin, ranges::end, ranges::starts_with, ranges::iterator_t;

        if constexpr (!is_permissive) {
            (void) starts_with(in1, in2);
            (void) starts_with(begin(in1), end(in1), begin(in2), end(in2));
        }

        BinaryPredicateFor<iterator_t<In1>, iterator_t<In2>> pred{};
        (void) starts_with(in1, in2, pred);
        (void) starts_with(begin(in1), end(in1), begin(in2), end(in2), pred);

        HalfProjectedBinaryPredicateFor<iterator_t<In2>> halfpred{};
        ProjectionFor<iterator_t<In1>> halfproj{};
        (void) starts_with(in1, in2, halfpred, halfproj);
        (void) starts_with(begin(in1), end(in1), begin(in2), end(in2), halfpred, halfproj);

        ProjectedBinaryPredicate<0, 1> projpred{};
        ProjectionFor<iterator_t<In1>, 0> proj1{};
        ProjectionFor<iterator_t<In2>, 1> proj2{};
        (void) starts_with(in1, in2, projpred, proj1, proj2);
        (void) starts_with(begin(in1), end(in1), begin(in2), end(in2), projpred, proj1, proj2);
    }
};

template void test_in_in<instantiator, const int, const int>();
#endif // TRANSITION, GH-1030
