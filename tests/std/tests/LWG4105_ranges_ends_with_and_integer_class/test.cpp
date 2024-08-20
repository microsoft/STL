// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <functional>
#include <ranges>
#include <span>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

template <class T>
concept testable_range = ranges::input_range<T> && (ranges::forward_range<T> || ranges::sized_range<T>);

template <class T>
concept testable_sentinel =
    ranges::input_range<T>
    && (ranges::forward_range<T> || sized_sentinel_for<ranges::sentinel_t<T>, ranges::iterator_t<T>>);

struct instantiator {
    static constexpr pair<int, int> haystack[]       = {{0, 42}, {1, 42}, {2, 42}, {4, 42}};
    static constexpr pair<int, int> short_haystack[] = {{4, 42}};
    static constexpr pair<long, long> long_needle[]  = {{13, 1}, {13, 2}, {13, 4}};
    static constexpr pair<long, long> short_needle[] = {{13, 2}, {13, 4}};
    static constexpr pair<long, long> wrong_needle[] = {{13, 2}, {13, 3}};

    template <std::_Signed_integer_like Diff1, testable_range In1, std::_Signed_integer_like Diff2, testable_range In2>
    static constexpr void test_range_rediff() {
        using ranges::ends_with, ranges::equal_to;

        {
            In1 r1{haystack};
            In2 r2{long_needle};
            const same_as<bool> auto match = ends_with(test::make_redifference_subrange<Diff1>(r1),
                test::make_redifference_subrange<Diff2>(r2), equal_to{}, get_first, get_second);
            assert(match);
        }
        {
            In1 r1{haystack};
            In2 r2{short_needle};
            const same_as<bool> auto match = ends_with(test::make_redifference_subrange<Diff1>(r1),
                test::make_redifference_subrange<Diff2>(r2), equal_to{}, get_first, get_second);
            assert(match);
        }
        {
            In1 r1{haystack};
            In2 r2{wrong_needle};
            const same_as<bool> auto match = ends_with(test::make_redifference_subrange<Diff1>(r1),
                test::make_redifference_subrange<Diff2>(r2), equal_to{}, get_first, get_second);
            assert(!match);
        }
        {
            In1 r1{short_haystack};
            In2 r2{short_needle};
            const same_as<bool> auto match = ends_with(test::make_redifference_subrange<Diff1>(r1),
                test::make_redifference_subrange<Diff2>(r2), equal_to{}, get_first, get_second);
            assert(!match);
        }
        {
            In1 r1{haystack};
            In2 r2{span<pair<long, long>, 0>{}};
            const same_as<bool> auto match = ends_with(test::make_redifference_subrange<Diff1>(r1),
                test::make_redifference_subrange<Diff2>(r2), equal_to{}, get_first, get_second);
            assert(match);
        }
    }

    template <ranges::input_range In1, ranges::input_range In2>
    static void call() {
        if constexpr (testable_range<In1> && testable_range<In2>) {
            using int_class = ranges::range_difference_t<ranges::iota_view<long long>>;

            test_range_rediff<int_class, In1, short, In2>();
            static_assert((test_range_rediff<int_class, In1, short, In2>(), true));

            test_range_rediff<short, In1, int_class, In2>();
            static_assert((test_range_rediff<short, In1, int_class, In2>(), true));
        }
    }
};

#ifdef TEST_EVERYTHING
int main() {
#if !defined(_PREFAST_) && !defined(__EDG__) // TRANSITION, GH-1030 and GH-3567
    test_in_in<instantiator, const pair<int, int>, const pair<long, long>>();
#endif // ^^^ no workaround ^^^
}
#else // ^^^ test all permutations of range properties / test only interesting permutations vvv
template <class R>
void run_tests_inner() {
    instantiator::call<R, test::range<test::input, const pair<long, long>, test::Sized::yes>>();
    instantiator::call<R, test::range<test::fwd, const pair<long, long>, test::Sized::no>>();
    instantiator::call<R, test::range<test::fwd, const pair<long, long>, test::Sized::yes>>();
    instantiator::call<R, test::range<test::bidi, const pair<long, long>, test::Sized::no>>();
    instantiator::call<R, test::range<test::bidi, const pair<long, long>, test::Sized::yes>>();
    instantiator::call<R, test::range<test::random, const pair<long, long>, test::Sized::no>>();
    instantiator::call<R, test::range<test::random, const pair<long, long>, test::Sized::yes>>();
    instantiator::call<R, test::range<test::contiguous, const pair<long, long>, test::Sized::no>>();
    instantiator::call<R, test::range<test::contiguous, const pair<long, long>, test::Sized::yes>>();
}

void run_tests() {
    run_tests_inner<test::range<test::input, const pair<int, int>, test::Sized::yes>>();
    run_tests_inner<test::range<test::fwd, const pair<int, int>, test::Sized::no>>();
    run_tests_inner<test::range<test::fwd, const pair<int, int>, test::Sized::yes>>();
    run_tests_inner<test::range<test::bidi, const pair<int, int>, test::Sized::no>>();
    run_tests_inner<test::range<test::bidi, const pair<int, int>, test::Sized::yes>>();
    run_tests_inner<test::range<test::random, const pair<int, int>, test::Sized::no>>();
    run_tests_inner<test::range<test::random, const pair<int, int>, test::Sized::yes>>();
    run_tests_inner<test::range<test::contiguous, const pair<int, int>, test::Sized::no>>();
    run_tests_inner<test::range<test::contiguous, const pair<int, int>, test::Sized::yes>>();
}

int main() {
    run_tests();
}
#endif // TEST_EVERYTHING
