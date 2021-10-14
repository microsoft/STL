// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>
using namespace std;

#include <range_algorithm_support.hpp>

struct instantiator {
    static constexpr pair<int, int> haystack[]       = {{0, 42}, {2, 42}, {4, 42}};
    static constexpr pair<int, int> short_haystack[] = {{0, 42}};
    static constexpr pair<long, long> needle[]       = {{13, 0}, {13, 2}};
    static constexpr pair<long, long> wrong_needle[] = {{13, 0}, {13, 3}};

    template <ranges::input_range In1, ranges::input_range In2>
    static constexpr void test() {
        using ranges::begin, ranges::end, ranges::starts_with;

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
        { // Validate infinite ranges
            const same_as<bool> auto infinite_haystack = starts_with(views::iota(0), views::iota(0, 5));
            assert(infinite_haystack);

            const same_as<bool> auto infinite_needle = starts_with(views::iota(0, 5), views::iota(0));
            assert(!infinite_needle);
        }
        { // Validate sized iterator + sentinel pairs
            const same_as<bool> auto match = starts_with(
                begin(haystack), end(haystack), begin(needle), end(needle), equal_to{}, get_first, get_second);
            assert(match);

            const same_as<bool> auto no_match = starts_with(begin(haystack), end(haystack), begin(wrong_needle),
                end(wrong_needle), equal_to{}, get_first, get_second);
            assert(!no_match);

            const same_as<bool> auto no_match2 = starts_with(begin(short_haystack), end(short_haystack), begin(needle),
                end(needle), equal_to{}, get_first, get_second);
            assert(!no_match2);
        }
        { // Validate non-sized iterator + sentinel pairs
            basic_borrowed_range wrapped_haystack{haystack};
            basic_borrowed_range wrapped_needle{needle};
            const same_as<bool> auto match = starts_with(begin(wrapped_haystack), end(wrapped_haystack),
                begin(wrapped_needle), end(wrapped_needle), equal_to{}, get_first, get_second);
            assert(match);

            basic_borrowed_range wrapped_haystack2{haystack};
            basic_borrowed_range wrapped_wrong_needle{wrong_needle};
            const same_as<bool> auto no_match = starts_with(begin(wrapped_haystack2), end(wrapped_haystack2),
                begin(wrapped_wrong_needle), end(wrapped_wrong_needle), equal_to{}, get_first, get_second);
            assert(!no_match);

            basic_borrowed_range wrapped_short_haystack{short_haystack};
            basic_borrowed_range wrapped_needle2{needle};
            const same_as<bool> auto no_match2 = starts_with(begin(wrapped_short_haystack), end(wrapped_short_haystack),
                begin(wrapped_needle2), end(wrapped_needle2), equal_to{}, get_first, get_second);
            assert(!no_match2);
        }
        { // Validate unreachable sentinels
            basic_borrowed_range wrapped_haystack{haystack};
            basic_borrowed_range wrapped_needle{needle};
            const same_as<bool> auto unreachable_haystack = starts_with(begin(wrapped_haystack), unreachable_sentinel,
                begin(wrapped_needle), end(wrapped_needle), equal_to{}, get_first, get_second);
            assert(unreachable_haystack);

            basic_borrowed_range wrapped_short_haystack{short_haystack};
            basic_borrowed_range wrapped_needle2{needle};
            const same_as<bool> auto unreachable_needle =
                starts_with(begin(wrapped_short_haystack), end(wrapped_short_haystack), begin(wrapped_needle2),
                    unreachable_sentinel, equal_to{}, get_first, get_second);
            assert(!unreachable_needle);
        }
    }

    template <ranges::input_range In1, ranges::input_range In2>
    static void call() {
        test<In1, In2>();
        STATIC_ASSERT((test<In1, In2>(), true));
    }
};

int main() {
#ifndef _PREFAST_ // TRANSITION, GH-1030
    test_in_in<instantiator, const pair<int, int>, const pair<int, int>>();
#endif // TRANSITION, GH-1030
}
