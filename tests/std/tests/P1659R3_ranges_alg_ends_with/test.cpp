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
    static constexpr pair<int, int> short_haystack[] = {{4, 42}};
    static constexpr pair<long, long> needle[]       = {{13, 2}, {13, 4}};
    static constexpr pair<long, long> wrong_needle[] = {{13, 2}, {13, 3}};

    template <ranges::input_range In1, ranges::input_range In2>
    static constexpr void test() {
        using ranges::begin, ranges::end, ranges::ends_with, ranges::forward_range, ranges::sized_range;

        if constexpr ((forward_range<In1> || sized_range<In1>) &&(forward_range<In2> || sized_range<In2>) ) {
            { // Validate ranges
                const same_as<bool> auto match = ends_with(haystack, needle, equal_to{}, get_first, get_second);
                assert(match);

                const same_as<bool> auto no_match =
                    ends_with(haystack, wrong_needle, equal_to{}, get_first, get_second);
                assert(!no_match);

                const same_as<bool> auto no_match2 =
                    ends_with(short_haystack, needle, equal_to{}, get_first, get_second);
                assert(!no_match2);
            }
            { // Validate iterator + sentinel pairs
                const same_as<bool> auto match = ends_with(
                    begin(haystack), end(haystack), begin(needle), end(needle), equal_to{}, get_first, get_second);
                assert(match);

                const same_as<bool> auto no_match = ends_with(begin(haystack), end(haystack), begin(wrong_needle),
                    end(wrong_needle), equal_to{}, get_first, get_second);
                assert(!no_match);

                const same_as<bool> auto no_match2 = ends_with(begin(short_haystack), end(short_haystack),
                    begin(needle), end(needle), equal_to{}, get_first, get_second);
                assert(!no_match2);
            }
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
