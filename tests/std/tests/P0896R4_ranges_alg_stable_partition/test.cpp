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

constexpr auto is_even = [](int i) { return i % 2 == 0; };

// Validate dangling story
static_assert(same_as<decltype(ranges::stable_partition(borrowed<false>{}, is_even)), ranges::dangling>);
static_assert(same_as<decltype(ranges::stable_partition(borrowed<true>{}, is_even)), ranges::subrange<int*>>);

struct instantiator {
    template <ranges::bidirectional_range Range>
    static void call() {
        using ranges::is_partitioned, ranges::is_sorted, ranges::iterator_t, ranges::stable_partition, ranges::subrange;

        { // Validate range overload
            P input[] = {P{0, 1}, P{1, 2}, P{0, 3}, P{1, 4}, P{0, 5}, P{1, 6}, P{0, 7}, P{1, 8}};
            Range range{input};
            const auto mid                                         = ranges::next(range.begin(), 4);
            const same_as<subrange<iterator_t<Range>>> auto result = stable_partition(range, is_even, get_first);
            assert(result.begin() == mid);
            assert(result.end() == range.end());
            assert(is_partitioned(range, is_even, get_first));
            assert(is_sorted(range));

            // Validate empty range
            const Range empty_range{span<P, 0>{}};
            const same_as<subrange<iterator_t<Range>>> auto empty_result =
                stable_partition(empty_range, is_even, get_first);
            assert(empty_result.begin() == empty_range.end());
            assert(empty_result.end() == empty_range.end());
        }

        { // Validate iterator overload
            P input[] = {P{0, 1}, P{1, 2}, P{0, 3}, P{1, 4}, P{0, 5}, P{1, 6}, P{0, 7}, P{1, 8}};
            Range range{input};
            const auto mid = ranges::next(range.begin(), 4);
            const same_as<subrange<iterator_t<Range>>> auto result =
                stable_partition(range.begin(), range.end(), is_even, get_first);
            assert(result.begin() == mid);
            assert(result.end() == range.end());
            assert(is_partitioned(range, is_even, get_first));
            assert(is_sorted(range));

            // Validate empty range
            const Range empty_range{span<P, 0>{}};
            const same_as<subrange<iterator_t<Range>>> auto empty_result =
                stable_partition(empty_range.begin(), empty_range.end(), is_even, get_first);
            assert(empty_result.begin() == empty_range.end());
            assert(empty_result.end() == empty_range.end());
        }
    }
};

int main() {
    test_bidi<instantiator, P>();
}
