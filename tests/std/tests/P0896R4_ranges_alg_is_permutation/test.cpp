// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

using P = pair<int, int>;

struct instantiator {
    static constexpr array<P, 8> pairs = {
        P{0, 42}, P{0, 42}, P{1, 42}, P{1, 42}, P{0, 42}, P{0, 42}, P{1, 42}, P{1, 42}};
    static constexpr array<int, 8> not_pairs     = {0, 0, 0, 0, 1, 1, 1, 1};
    static constexpr array<int, 8> too_few_ones  = {0, 0, 0, 0, 0, 1, 1, 1};
    static constexpr array<int, 8> too_many_ones = {0, 0, 0, 1, 1, 1, 1, 1};

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

        {
            // negative case [different lengths]
            const Fwd2 suffix2{span{not_pairs}.subspan<2>()};
            assert(!ranges::is_permutation(range1, suffix2, ranges::equal_to{}, get_first, identity{}));
            assert(!ranges::is_permutation(suffix2, range1, ranges::equal_to{}, identity{}, get_first));
            assert(!ranges::is_permutation(ranges::begin(range1), ranges::end(range1), ranges::begin(suffix2),
                ranges::end(suffix2), ranges::equal_to{}, get_first, identity{}));
            assert(!ranges::is_permutation(ranges::begin(suffix2), ranges::end(suffix2), ranges::begin(range1),
                ranges::end(range1), ranges::equal_to{}, identity{}, get_first));
        }
        {
            // negative case [1 appears more in first range than second]
            const Fwd2 r2{too_few_ones};
            assert(!ranges::is_permutation(range1, r2, ranges::equal_to{}, get_first, identity{}));
            assert(!ranges::is_permutation(ranges::begin(range1), ranges::end(range1), ranges::begin(r2),
                ranges::end(r2), ranges::equal_to{}, get_first, identity{}));
        }
        {
            // negative case [1 appears more in second range than first]
            const Fwd2 r2{too_many_ones};
            assert(!ranges::is_permutation(range1, r2, ranges::equal_to{}, get_first, identity{}));
            assert(!ranges::is_permutation(ranges::begin(range1), ranges::end(range1), ranges::begin(r2),
                ranges::end(r2), ranges::equal_to{}, get_first, identity{}));
        }

        {
            // negative case [only final elements differ]
            const Fwd1 r1{span{pairs}.subspan<0, 3>()};
            const Fwd2 r2{span{not_pairs}.subspan<0, 3>()};
            assert(!ranges::is_permutation(r1, r2, ranges::equal_to{}, get_first, identity{}));
            assert(!ranges::is_permutation(ranges::begin(r1), ranges::end(r1), ranges::begin(r2), ranges::end(r2),
                ranges::equal_to{}, get_first, identity{}));
        }
        {
            // negative case [only initial elements differ]
            const Fwd1 r1{span{pairs}.subspan<1, 3>()};
            const Fwd2 r2{span{not_pairs}.subspan<4, 3>()};
            assert(!ranges::is_permutation(r1, r2, ranges::equal_to{}, get_first, identity{}));
            assert(!ranges::is_permutation(ranges::begin(r1), ranges::end(r1), ranges::begin(r2), ranges::end(r2),
                ranges::equal_to{}, get_first, identity{}));
        }
        {
            int arr1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            int arr2[] = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
            const Fwd2 r1{arr1};
            const Fwd2 r2{arr2};
            assert(ranges::is_permutation(r1, r2));
        }
        {
            int arr1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            int arr2[] = {9, 8, 7, 3, 4, 5, 6, 2, 1, 0};
            const Fwd2 r1{arr1};
            const Fwd2 r2{arr2};
            assert(ranges::is_permutation(r1, r2));
        }
        {
            int arr1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            int arr2[] = {9, 1, 7, 3, 5, 4, 6, 2, 8, 0};
            const Fwd2 r1{arr1};
            const Fwd2 r2{arr2};
            assert(ranges::is_permutation(r1, r2));
        }
        {
            int arr1[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            int arr2[] = {9, 1, 7, 5, 6, 3, 4, 2, 8, 0};
            const Fwd2 r1{arr1};
            const Fwd2 r2{arr2};
            assert(ranges::is_permutation(r1, r2));
        }
        {
            int arr1[] = {0, 1, 2, 3, 4, 10, 5, 6, 7, 8, 9};
            int arr2[] = {9, 1, 7, 3, 5, 11, 4, 6, 2, 8, 0};
            const Fwd2 r1{arr1};
            const Fwd2 r2{arr2};
            assert(!ranges::is_permutation(r1, r2));
        }
        {
            int arr1[] = {-1, 0, 1, 2, 3, 4, 5, 6, 7, 8};
            int arr2[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
            const Fwd2 r1{arr1};
            const Fwd2 r2{arr2};
            assert(!ranges::is_permutation(r1, r2));
            assert(!ranges::is_permutation(r1, r2, {}, {}, [](int n) { return n - 1; }));
            assert(ranges::is_permutation(r1, r2, {}, {}, [](int n) { return n - 2; }));
            assert(ranges::is_permutation(
                r1, r2, {}, [](int n) { return n + 1; }, [](int n) { return n - 1; }));
        }
        { // Test GH-2888: `<algorithm>`: `ranges::is_permutation`'s helper lambda does not specify return type
            struct NonCopyableBool {
                constexpr operator bool() {
                    return true;
                }

                NonCopyableBool()                       = default;
                NonCopyableBool(const NonCopyableBool&) = delete;
            };
            NonCopyableBool b;
            assert(ranges::is_permutation(range2, range2, [&](auto, auto) -> NonCopyableBool& { return b; }));
        }
    }
};

#ifdef TEST_EVERYTHING
int main() {
    // No constexpr test here; this test_fwd_fwd call exceeds the maximum number of steps in a constexpr computation.
    test_fwd_fwd<instantiator, const P, const int>();
}
#else // ^^^ test all range combinations / test only interesting range combos vvv
template <class Elem, test::Sized IsSized>
using fwd_test_range = test::range<forward_iterator_tag, Elem, IsSized, test::CanDifference::no, test::Common::no,
    test::CanCompare::yes, test::ProxyRef::yes>;
template <class Elem, test::Sized IsSized, test::Common Eq>
using bidi_test_range = test::range<bidirectional_iterator_tag, Elem, IsSized, test::CanDifference::no, Eq,
    test::CanCompare::yes, test::ProxyRef::yes>;

constexpr bool run_tests() {
    using Elem1 = const P;
    using Elem2 = const int;

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

    return true;
}

int main() {
    STATIC_ASSERT(run_tests());
    run_tests();
}
#endif // TEST_EVERYTHING
