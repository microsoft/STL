// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;

using P = pair<int, int>;

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::search(borrowed<false>{}, borrowed<true>{})), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::search(borrowed<true>{}, borrowed<false>{})), ranges::subrange<int*>>);

struct instantiator {
    static constexpr array<P, 8> pairs = {
        P{0, 42}, P{1, 42}, P{2, 42}, P{3, 42}, P{4, 42}, P{5, 42}, P{6, 42}, P{7, 42}};
    static constexpr array<int, 3> not_pairs     = {2, 3, 4};
    static constexpr array<int, 3> neg_not_pairs = {-2, -3, -4};

    template <class Fwd1, class Fwd2>
    static constexpr void call() {
        const Fwd1 range1{pairs};
        {
            const Fwd2 range2{not_pairs};

            // defaulted predicate and projections
            {
                auto result = ranges::search(range1, range1);
                STATIC_ASSERT(same_as<decltype(result), ranges::subrange<ranges::iterator_t<Fwd1>>>);
                assert(result.begin() == range1.begin());
                assert(result.end() == range1.end());
            }
            {
                auto result = ranges::search(
                    ranges::begin(range2), ranges::end(range2), ranges::begin(range2), ranges::end(range2));
                STATIC_ASSERT(same_as<decltype(result), ranges::subrange<ranges::iterator_t<Fwd2>>>);
                assert(result.begin() == range2.begin());
                assert(result.end() == range2.end());
            }
        }

        const Fwd2 range2{neg_not_pairs};
        const auto pred = [](int x, int y) { return x == -y; };

        // explicit predicate
        {
            auto result = ranges::search(range2, range2, ranges::equal_to{});
            STATIC_ASSERT(same_as<decltype(result), ranges::subrange<ranges::iterator_t<Fwd2>>>);
            assert(result.begin() == range2.begin());
            assert(result.end() == range2.end());
        }
        {
            auto result = ranges::search(ranges::begin(range1), ranges::end(range1), ranges::begin(range1),
                ranges::end(range1), ranges::equal_to{});
            STATIC_ASSERT(same_as<decltype(result), ranges::subrange<ranges::iterator_t<Fwd1>>>);
            assert(result.begin() == range1.begin());
            assert(result.end() == range1.end());
        }

        // explicit predicate and one projection
        {
            auto result = ranges::search(range1, range2, pred, get_first);
            STATIC_ASSERT(same_as<decltype(result), ranges::subrange<ranges::iterator_t<Fwd1>>>);
            assert(result.begin() == ranges::next(range1.begin(), 2));
            assert(result.end() == ranges::next(range1.begin(), 5));
        }
        {
            auto result = ranges::search(ranges::begin(range1), ranges::end(range1), ranges::begin(range2),
                ranges::end(range2), pred, get_first);
            STATIC_ASSERT(same_as<decltype(result), ranges::subrange<ranges::iterator_t<Fwd1>>>);
            assert(result.begin() == ranges::next(range1.begin(), 2));
            assert(result.end() == ranges::next(range1.begin(), 5));
        }

        // explicit predicate and two projections
        constexpr auto minus1 = [](int x) { return x - 1; };
        {
            auto result = ranges::search(range1, range2, pred, get_first, minus1);
            STATIC_ASSERT(same_as<decltype(result), ranges::subrange<ranges::iterator_t<Fwd1>>>);
            assert(result.begin() == ranges::next(range1.begin(), 3));
            assert(result.end() == ranges::next(range1.begin(), 6));
        }
        {
            auto result = ranges::search(ranges::begin(range1), ranges::end(range1), ranges::begin(range2),
                ranges::end(range2), pred, get_first, minus1);
            STATIC_ASSERT(same_as<decltype(result), ranges::subrange<ranges::iterator_t<Fwd1>>>);
            assert(result.begin() == ranges::next(range1.begin(), 3));
            assert(result.end() == ranges::next(range1.begin(), 6));
        }

        // negative case
        {
            auto result = ranges::search(range2, range1, pred, minus1, get_first);
            STATIC_ASSERT(same_as<decltype(result), ranges::subrange<ranges::iterator_t<Fwd2>>>);
            assert(result.empty());
        }
        {
            auto result = ranges::search(ranges::begin(range2), ranges::end(range2), ranges::begin(range1),
                ranges::end(range1), pred, minus1, get_first);
            STATIC_ASSERT(same_as<decltype(result), ranges::subrange<ranges::iterator_t<Fwd2>>>);
            assert(result.empty());
        }
    }
};

using Elem1 = const P;
using Elem2 = const int;

#ifdef TEST_EVERYTHING
int main() {
    // No constexpr test here; the test_fwd_fwd call exceeds the maximum number of steps in a constexpr computation.
    test_fwd_fwd<instantiator, Elem1, Elem2>();
}
#else // ^^^ test all range combinations / test only interesting range combos vvv
template <class Elem, test::Sized IsSized>
using fwd_test_range = test::range<forward_iterator_tag, Elem, IsSized, test::CanDifference::no, test::Common::no,
    test::CanCompare::yes, test::ProxyRef::yes>;
template <class Elem, test::Sized IsSized, test::Common IsCommon>
using random_test_range = test::range<random_access_iterator_tag, Elem, IsSized, test::CanDifference::no, IsCommon,
    test::CanCompare::yes, test::ProxyRef::no>;

constexpr bool run_tests() {
    // All (except contiguous) proxy reference types, since the algorithm doesn't really care. Cases with only 1 range
    // sized are not interesting; common is interesting only in that it's necessary to trigger memcmp optimization.

    using test::Common, test::Sized;

    // both forward, non-common, and sized or unsized
    instantiator::call<fwd_test_range<Elem1, Sized::no>, fwd_test_range<Elem2, Sized::no>>();
    instantiator::call<fwd_test_range<Elem1, Sized::yes>, fwd_test_range<Elem2, Sized::yes>>();

    // both random-access, and sized or unsized; all permutations of common
    instantiator::call<random_test_range<Elem1, Sized::no, Common::no>,
        random_test_range<Elem2, Sized::no, Common::no>>();
    instantiator::call<random_test_range<Elem1, Sized::no, Common::no>,
        random_test_range<Elem2, Sized::no, Common::yes>>();
    instantiator::call<random_test_range<Elem1, Sized::no, Common::yes>,
        random_test_range<Elem2, Sized::no, Common::no>>();
    instantiator::call<random_test_range<Elem1, Sized::no, Common::yes>,
        random_test_range<Elem2, Sized::no, Common::yes>>();
    instantiator::call<random_test_range<Elem1, Sized::yes, Common::no>,
        random_test_range<Elem2, Sized::yes, Common::no>>();
    instantiator::call<random_test_range<Elem1, Sized::yes, Common::no>,
        random_test_range<Elem2, Sized::yes, Common::yes>>();
    instantiator::call<random_test_range<Elem1, Sized::yes, Common::yes>,
        random_test_range<Elem2, Sized::yes, Common::no>>();
    instantiator::call<random_test_range<Elem1, Sized::yes, Common::yes>,
        random_test_range<Elem2, Sized::yes, Common::yes>>();

    {
        // Validate the memcmp optimization
        const int haystack[] = {1, 2, 3, 1, 2, 3, 1, 2, 3};
        const int needle[]   = {1, 2, 3};
        const auto result    = ranges::search(span<const int>{haystack}, needle);
        STATIC_ASSERT(same_as<decltype(result), const ranges::subrange<span<const int>::iterator>>);
        assert(to_address(result.begin()) == haystack + 0);
        assert(to_address(result.end()) == haystack + 3);
    }

    return true;
}

constexpr void test_devcom_1559808() {
    // Regression test for DevCom-1559808, an interaction between vector and the
    // use of structured bindings in the constexpr evaluator.

    vector<int> haystack(33, 42); // No particular significance to any numbers in this function
    vector<int> needle(8, 42);

    auto result = ranges::search(haystack, needle);
    assert(result.begin() == haystack.begin());
    assert(result.end() == haystack.begin() + ranges::ssize(needle));

    needle.assign(6, 1729);
    result = ranges::search(haystack, needle);
    assert(result.begin() == haystack.end());
    assert(result.end() == haystack.end());
}

int main() {
    STATIC_ASSERT(run_tests());
    run_tests();

    STATIC_ASSERT((test_devcom_1559808(), true));
    test_devcom_1559808();
}
#endif // TEST_EVERYTHING
