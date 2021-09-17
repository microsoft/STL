// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>
#include <utility>
using namespace std;

#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::dangling, ranges::equal_to, ranges::iterator_t, ranges::ends_with;

    constexpr array<pair<int, int>, 3> haystack       = {{{0, 42}, {2, 42}, {4, 42}}};
    constexpr array<pair<int, int>, 1> short_haystack = {{{4, 42}}};
    constexpr array<pair<long, long>, 2> needle       = {{{13, 2}, {13, 4}}};
    constexpr array<pair<long, long>, 2> wrong_needle = {{{13, 2}, {13, 3}}};

    { // Validate sized ranges
        const same_as<bool> auto match = ends_with(haystack, needle, equal_to{}, get_first, get_second);
        assert(match);

        const same_as<bool> auto no_match = ends_with(haystack, wrong_needle, equal_to{}, get_first, get_second);
        assert(!no_match);

        const same_as<bool> auto no_match2 = ends_with(short_haystack, needle, equal_to{}, get_first, get_second);
        assert(!no_match2);
    }
    { // Validate infinite ranges
        const same_as<bool> auto infinite_needle = ends_with(views::iota(0, 5), views::iota(0));
        assert(!infinite_needle);
    }
    { // Validate sized iterator + sentinel pairs
        const same_as<bool> auto match = ends_with(
            haystack.begin(), haystack.end(), needle.begin(), needle.end(), equal_to{}, get_first, get_second);
        assert(match);

        const same_as<bool> auto no_match = ends_with(haystack.begin(), haystack.end(), wrong_needle.begin(),
            wrong_needle.end(), equal_to{}, get_first, get_second);
        assert(!no_match);

        const same_as<bool> auto no_match2 = ends_with(short_haystack.begin(), short_haystack.end(), needle.begin(),
            needle.end(), equal_to{}, get_first, get_second);
        assert(!no_match2);
    }
    { // Validate unreachable sentinel
        const same_as<bool> auto unreachable_needle = ends_with(
            haystack.begin(), haystack.end(), needle.begin(), unreachable_sentinel, equal_to{}, get_first, get_second);
        assert(!unreachable_needle);
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

#ifndef _PREFAST_ // TRANSITION, GH-1030
struct instantiator {
    template <class In1, class In2>
    static void call() {
        using ranges::begin, ranges::end, ranges::ends_with, ranges::iterator_t, ranges::forward_range,
            ranges::sized_range, ranges::sentinel_t;

        In1 in1{span<const int, 0>{}};
        In2 in2{span<const int, 0>{}};

        if constexpr ((forward_range<In1> || sized_range<In1>) &&(forward_range<In2> || sized_range<In2>) ) {
            if constexpr (!is_permissive) {
                (void) ends_with(in1, in2);
            }

            BinaryPredicateFor<iterator_t<In1>, iterator_t<In2>> pred{};
            (void) ends_with(in1, in2, pred);

            HalfProjectedBinaryPredicateFor<iterator_t<In2>> halfpred{};
            ProjectionFor<iterator_t<In1>> halfproj{};
            (void) ends_with(in1, in2, halfpred, halfproj);

            ProjectedBinaryPredicate<0, 1> projpred{};
            ProjectionFor<iterator_t<In1>, 0> proj1{};
            ProjectionFor<iterator_t<In2>, 1> proj2{};
            (void) ends_with(in1, in2, projpred, proj1, proj2);
        }

        if constexpr ((forward_iterator<iterator_t<In1>> || sized_sentinel_for<sentinel_t<In1>, iterator_t<In1>>) &&(
                          forward_iterator<iterator_t<In2>> || sized_sentinel_for<sentinel_t<In1>, iterator_t<In2>>) ) {
            if constexpr (!is_permissive) {
                (void) ends_with(begin(in1), end(in1), begin(in2), end(in2));
            }

            BinaryPredicateFor<iterator_t<In1>, iterator_t<In2>> pred{};
            (void) ends_with(begin(in1), end(in1), begin(in2), end(in2), pred);

            HalfProjectedBinaryPredicateFor<iterator_t<In2>> halfpred{};
            ProjectionFor<iterator_t<In1>> halfproj{};
            (void) ends_with(begin(in1), end(in1), begin(in2), end(in2), halfpred, halfproj);

            ProjectedBinaryPredicate<0, 1> projpred{};
            ProjectionFor<iterator_t<In1>, 0> proj1{};
            ProjectionFor<iterator_t<In2>, 1> proj2{};
            (void) ends_with(begin(in1), end(in1), begin(in2), end(in2), projpred, proj1, proj2);
        }
    }
};

template void test_in_in<instantiator, const int, const int>();
#endif // TRANSITION, GH-1030
