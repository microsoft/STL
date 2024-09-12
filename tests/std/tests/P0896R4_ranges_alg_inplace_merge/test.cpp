// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

#include <range_algorithm_support.hpp>

using namespace std;
using P = pair<int, int>;
// Validate dangling story
static_assert(same_as<decltype(ranges::inplace_merge(borrowed<false>{}, nullptr_to<int>)), ranges::dangling>);
static_assert(same_as<decltype(ranges::inplace_merge(borrowed<true>{}, nullptr_to<int>)), int*>);

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

// Test GH-4863: <algorithm>: ranges::inplace_merge doesn't seem to be able to utilize ranges::upper_bound
void test_gh_4863() { // COMPILE-ONLY
    {
        vector<int> v;
        auto cmp = [](int&, int&) { return false; };
        ranges::sort(v, cmp);
        ranges::inplace_merge(v, v.begin(), cmp);
    }
    {
        struct S {
            operator nullptr_t() {
                return nullptr;
            }
        };
        vector<int> v;
        auto cmp = [](const nullptr_t&, const nullptr_t&) { return false; };
        ranges::inplace_merge(v, v.begin(), cmp, [](int) { return S{}; });
    }
}

int main() {
    test_bidi<instantiator, P>();
}
