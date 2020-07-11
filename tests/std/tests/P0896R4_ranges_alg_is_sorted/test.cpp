// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Covers both ranges::is_sorted and ranges::is_sorted_until

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::is_sorted_until(borrowed<false>{})), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::is_sorted_until(borrowed<true>{})), int*>);

using P = pair<int, int>;

static constexpr array<P, 8> pairs = {P{0, 80}, P{1, 70}, P{2, 60}, P{3, 50}, P{4, 40}, P{5, 30}, P{6, 20}, P{7, 10}};

struct range_overloads {
    template <class Fwd>
    static constexpr void call() {
        constexpr auto N = static_cast<int>(pairs.size());
        auto elements    = pairs;
        const Fwd range{elements};
        for (int offset = 0; offset < N; ++offset) {
            const bool sorted = offset == 0;
            assert(ranges::is_sorted(range) == sorted);
            assert(ranges::is_sorted(range, ranges::less{}) == sorted);
            assert(ranges::is_sorted(range, ranges::less{}, get_first) == sorted);

            const auto addr = elements.data() + (N - offset);
            assert(ranges::is_sorted_until(range).peek() == addr);
            assert(ranges::is_sorted_until(range, ranges::less{}).peek() == addr);
            assert(ranges::is_sorted_until(range, ranges::less{}, get_first).peek() == addr);

            const auto next = ranges::next(ranges::begin(elements));
            rotate(ranges::begin(elements), next, ranges::end(elements));
        }
    }
};

struct iterator_overloads {
    template <class Fwd>
    static constexpr void call() {
        using ranges::begin, ranges::end;
        constexpr auto N = static_cast<int>(pairs.size());
        auto elements    = pairs;
        const Fwd range{elements};
        for (int offset = 0; offset < N; ++offset) {
            const bool sorted = offset == 0;
            assert(ranges::is_sorted(begin(range), end(range)) == sorted);
            assert(ranges::is_sorted(begin(range), end(range), ranges::less{}) == sorted);
            assert(ranges::is_sorted(begin(range), end(range), ranges::less{}, get_first) == sorted);

            const auto addr = elements.data() + (N - offset);
            assert(ranges::is_sorted_until(begin(range), end(range)).peek() == addr);
            assert(ranges::is_sorted_until(begin(range), end(range), ranges::less{}).peek() == addr);
            assert(ranges::is_sorted_until(begin(range), end(range), ranges::less{}, get_first).peek() == addr);

            const auto next = ranges::next(begin(elements));
            rotate(begin(elements), next, end(elements));
        }
    }
};

int main() {
    STATIC_ASSERT((test_fwd<range_overloads, const P>(), true));
    test_fwd<range_overloads, const P>();

    STATIC_ASSERT((test_fwd<iterator_overloads, const P>(), true));
    test_fwd<iterator_overloads, const P>();
}
