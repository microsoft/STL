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

// clang-format off
template <class T>
concept testable_sentinel = ranges::input_range<T>
    && (ranges::forward_range<T> || sized_sentinel_for<ranges::sentinel_t<T>, ranges::iterator_t<T>>);
// clang-format on

struct instantiator {
    static constexpr pair<int, int> haystack[]       = {{0, 42}, {1, 42}, {2, 42}, {4, 42}};
    static constexpr pair<int, int> short_haystack[] = {{4, 42}};
    static constexpr pair<long, long> long_needle[]  = {{13, 1}, {13, 2}, {13, 4}};
    static constexpr pair<long, long> short_needle[] = {{13, 2}, {13, 4}};
    static constexpr pair<long, long> wrong_needle[] = {{13, 2}, {13, 3}};

    template <testable_range In1, testable_range In2>
    static constexpr void test_range() {
        using ranges::ends_with, ranges::equal_to;

        {
            const same_as<bool> auto match =
                ends_with(In1{haystack}, In2{long_needle}, equal_to{}, get_first, get_second);
            assert(match);
        }
        {
            const same_as<bool> auto match =
                ends_with(In1{haystack}, In2{short_needle}, equal_to{}, get_first, get_second);
            assert(match);
        }
        {
            const same_as<bool> auto match =
                ends_with(In1{haystack}, In2{wrong_needle}, equal_to{}, get_first, get_second);
            assert(!match);
        }
        {
            const same_as<bool> auto match =
                ends_with(In1{short_haystack}, In2{short_needle}, equal_to{}, get_first, get_second);
            assert(!match);
        }
        {
            const same_as<bool> auto match =
                ends_with(In1{haystack}, In2{span<pair<long, long>, 0>{}}, equal_to{}, get_first, get_second);
            assert(match);
        }
    }

    template <testable_sentinel In1, testable_sentinel In2>
    static constexpr void test_iterator_sentinel() {
        using ranges::begin, ranges::end, ranges::ends_with, ranges::equal_to;

        {
            In1 h{haystack};
            In2 n{long_needle};
            const same_as<bool> auto match =
                ends_with(begin(h), end(h), begin(n), end(n), equal_to{}, get_first, get_second);
            assert(match);
        }
        {
            In1 h{haystack};
            In2 n{short_needle};
            const same_as<bool> auto match =
                ends_with(begin(h), end(h), begin(n), end(n), equal_to{}, get_first, get_second);
            assert(match);
        }
        {
            In1 h{haystack};
            In2 n{wrong_needle};
            const same_as<bool> auto match =
                ends_with(begin(h), end(h), begin(n), end(n), equal_to{}, get_first, get_second);
            assert(!match);
        }
        {
            In1 h{short_haystack};
            In2 n{short_needle};
            const same_as<bool> auto match =
                ends_with(begin(h), end(h), begin(n), end(n), equal_to{}, get_first, get_second);
            assert(!match);
        }
        {
            In1 h{haystack};
            In2 n{span<pair<long, long>, 0>{}};
            const same_as<bool> auto match =
                ends_with(begin(h), end(h), begin(n), end(n), equal_to{}, get_first, get_second);
            assert(match);
        }
    }

    template <ranges::input_range In1, ranges::input_range In2>
    static void call() {
        if constexpr (testable_range<In1> && testable_range<In2>) {
            test_range<In1, In2>();
            STATIC_ASSERT((test_range<In1, In2>(), true));
        }

        if constexpr (testable_sentinel<In1> && testable_sentinel<In2>) {
            test_iterator_sentinel<In1, In2>();
            STATIC_ASSERT((test_iterator_sentinel<In1, In2>(), true));
        }
    }
};

int main() {
#ifndef _PREFAST_ // TRANSITION, GH-1030
    test_in_in<instantiator, const pair<int, int>, const pair<long, long>>();
#endif // TRANSITION, GH-1030
}
