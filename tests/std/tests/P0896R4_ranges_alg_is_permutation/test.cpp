// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>
//
#include <range_algorithm_support.hpp>

using namespace std;

struct instantiator {
    using P = pair<int, int>;

    static constexpr array<P, 8> pairs = {
        P{0, 42}, P{1, 42}, P{0, 42}, P{1, 42}, P{0, 42}, P{1, 42}, P{0, 42}, P{1, 42}};
    static constexpr array<int, 8> not_pairs = {0, 0, 0, 0, 1, 1, 1, 1};

    template <class Fwd1, class Fwd2>
    static constexpr void call() {
        const Fwd1 range1{pairs};
        const Fwd2 range2{not_pairs};

        // defaulted predicate and projections
        assert(ranges::is_permutation(range1, range1));
        assert(ranges::is_permutation(
            ranges::begin(range2), ranges::end(range2), ranges::begin(range2), ranges::end(range2)));

        // explicit predicate
        assert(ranges::is_permutation(range2, range2, ranges::equal_to{}));
        assert(ranges::is_permutation(ranges::begin(range1), ranges::end(range1), ranges::begin(range1),
            ranges::end(range1), ranges::equal_to{}));

        // explicit predicate and one projection
        assert(ranges::is_permutation(range1, range2, ranges::equal_to{}, get_first));
        assert(ranges::is_permutation(ranges::begin(range1), ranges::end(range1), ranges::begin(range2),
            ranges::end(range2), ranges::equal_to{}, get_first));

        // explicit predicate and two projections
        assert(ranges::is_permutation(range1, range2, ranges::equal_to{}, get_first, identity{}));
        assert(ranges::is_permutation(ranges::begin(range1), ranges::end(range1), ranges::begin(range2),
            ranges::end(range2), ranges::equal_to{}, get_first, identity{}));

        // negative case
        assert(!ranges::is_permutation(range1, range2, ranges::equal_to{}, get_second, identity{}));
        assert(!ranges::is_permutation(ranges::begin(range1), ranges::end(range1), ranges::begin(range2),
            ranges::end(range2), ranges::equal_to{}, get_second, identity{}));
    }
};

template <class Elem, test::Sized IsSized>
using fwd_test_range = test::range<forward_iterator_tag, Elem, IsSized, test::CanDifference::no, test::Common::no,
    test::CanCompare::yes, test::ProxyRef::yes>;
template <class Elem, test::Sized IsSized, test::Common Eq>
using bidi_test_range = test::range<bidirectional_iterator_tag, Elem, IsSized, test::CanDifference::no, Eq,
    test::CanCompare::yes, test::ProxyRef::yes>;

constexpr bool run_tests() {
    using Elem1 = const pair<int, int>;
    using Elem2 = const int;

#ifdef TEST_EVERYTHING
    test_fwd_fwd<instantiator, Elem1, Elem2>();
#else // ^^^ test all range combinations // test only interesting range combos vvv
    // All proxy reference types, since the algorithm doesn't really care.
    using test::Common, test::Sized;

    // both forward non-common and 0-/1-/2-sized
    instantiator::call<fwd_test_range<Elem1, Sized::no>, fwd_test_range<Elem2, Sized::no>>();
    instantiator::call<fwd_test_range<Elem1, Sized::yes>, fwd_test_range<Elem2, Sized::no>>();
    instantiator::call<fwd_test_range<Elem1, Sized::no>, fwd_test_range<Elem2, Sized::yes>>();
    instantiator::call<fwd_test_range<Elem1, Sized::yes>, fwd_test_range<Elem2, Sized::yes>>();

    // both bidi same common and 0-/1-/2-sized

    instantiator::call<bidi_test_range<Elem1, Sized::no, Common::yes>, bidi_test_range<Elem2, Sized::no, Common::no>>();
    instantiator::call<bidi_test_range<Elem1, Sized::yes, Common::yes>,
        bidi_test_range<Elem2, Sized::no, Common::no>>();
    instantiator::call<bidi_test_range<Elem1, Sized::no, Common::yes>,
        bidi_test_range<Elem2, Sized::yes, Common::no>>();
    instantiator::call<bidi_test_range<Elem1, Sized::yes, Common::yes>,
        bidi_test_range<Elem2, Sized::yes, Common::no>>();

    instantiator::call<bidi_test_range<Elem1, Sized::no, Common::yes>,
        bidi_test_range<Elem2, Sized::no, Common::yes>>();
    instantiator::call<bidi_test_range<Elem1, Sized::yes, Common::yes>,
        bidi_test_range<Elem2, Sized::no, Common::yes>>();
    instantiator::call<bidi_test_range<Elem1, Sized::no, Common::yes>,
        bidi_test_range<Elem2, Sized::yes, Common::yes>>();
    instantiator::call<bidi_test_range<Elem1, Sized::yes, Common::yes>,
        bidi_test_range<Elem2, Sized::yes, Common::yes>>();
#endif // TEST_EVERYTHING

    return true;
}

int main() {
#ifndef TEST_EVERYTHING // This crushes the constexpr step limit on our compilers
    STATIC_ASSERT(run_tests());
#endif // TEST_EVERYTHING
    run_tests();
}
