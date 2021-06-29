// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <span>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;
using P = pair<int, int>;
// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::inplace_merge(borrowed<false>{}, nullptr_to<int>)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::inplace_merge(borrowed<true>{}, nullptr_to<int>)), int*>);

struct instantiator {
    static constexpr P expected[] = {P{0, 1}, P{0, 5}, P{4, 2}, P{4, 6}, P{6, 7}, P{7, 3}, P{8, 4}, P{9, 8}, P{10, 9}};

    template <ranges::bidirectional_range Range>
    static void call() {
        using ranges::equal, ranges::is_sorted, ranges::iterator_t, ranges::inplace_merge;

        { // Validate range overload
            P input[] = {P{0, 1}, P{4, 2}, P{7, 3}, P{8, 4}, P{0, 5}, P{4, 6}, P{6, 7}, P{9, 8}, P{10, 9}};
            Range range{input};
            const auto mid                               = ranges::next(range.begin(), 4);
            const same_as<iterator_t<Range>> auto result = inplace_merge(range, mid, ranges::less{}, get_first);
            assert(result == range.end());
            assert(equal(input, expected));

            // Validate empty range
            const Range empty_range{span<P, 0>{}};
            const same_as<iterator_t<Range>> auto empty_result =
                inplace_merge(empty_range, empty_range.begin(), ranges::less{}, get_first);
            assert(empty_result == empty_range.begin());
        }

        { // Validate iterator overload
            P input[] = {P{0, 1}, P{4, 2}, P{7, 3}, P{8, 4}, P{0, 5}, P{4, 6}, P{6, 7}, P{9, 8}, P{10, 9}};
            Range range{input};
            const auto mid = ranges::next(range.begin(), 4);
            const same_as<iterator_t<Range>> auto result =
                inplace_merge(range.begin(), mid, range.end(), ranges::less{}, get_first);
            assert(result == range.end());
            assert(equal(input, expected));

            // Validate empty range
            const Range empty_range{span<P, 0>{}};
            const same_as<iterator_t<Range>> auto empty_result =
                inplace_merge(empty_range.begin(), empty_range.begin(), empty_range.end(), ranges::less{}, get_first);
            assert(empty_result == empty_range.end());
        }
    }
};

int main() {
    test_bidi<instantiator, P>();
}
