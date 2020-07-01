// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Covers ranges::lower_bound, ranges::upper_bound, ranges::equal_range, and ranges::binary_search

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

#define ASSERT(...) assert((__VA_ARGS__))

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::lower_bound(borrowed<false>{}, 42)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::lower_bound(borrowed<true>{}, 42)), int*>);
STATIC_ASSERT(same_as<decltype(ranges::upper_bound(borrowed<false>{}, 42)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::upper_bound(borrowed<true>{}, 42)), int*>);
STATIC_ASSERT(same_as<decltype(ranges::equal_range(borrowed<false>{}, 42)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::equal_range(borrowed<true>{}, 42)), ranges::subrange<int*>>);

using P = pair<int, int>;

struct empty_ranges {
    template <ranges::forward_range Fwd>
    static constexpr void call() {
        // Validate empty ranges
        const Fwd range{};

        ASSERT(ranges::lower_bound(range, 42, ranges::less{}, get_first) == ranges::end(range));
        ASSERT(ranges::lower_bound(ranges::begin(range), ranges::end(range), 42, ranges::less{}, get_first)
               == ranges::end(range));

        ASSERT(ranges::upper_bound(range, 42, ranges::less{}, get_first) == ranges::end(range));
        ASSERT(ranges::upper_bound(ranges::begin(range), ranges::end(range), 42, ranges::less{}, get_first)
               == ranges::end(range));

        {
            auto result = ranges::equal_range(range, 42, ranges::less{}, get_first);
            ASSERT(result.begin() == ranges::end(range));
            ASSERT(result.end() == ranges::end(range));
        }
        {
            auto result = ranges::equal_range(ranges::begin(range), ranges::end(range), 42, ranges::less{}, get_first);
            ASSERT(result.begin() == ranges::end(range));
            ASSERT(result.end() == ranges::end(range));
        }

        ASSERT(!ranges::binary_search(range, 42, ranges::less{}, get_first));
        ASSERT(!ranges::binary_search(ranges::begin(range), ranges::end(range), 42, ranges::less{}, get_first));
    }
};

static constexpr array<P, 10> pairs = {
    P{0, 100}, P{1, 90}, P{2, 80}, P{3, 70}, P{3, 60}, P{3, 50}, P{4, 40}, P{5, 30}, P{6, 20}, P{7, 10}};

struct lower_bound_instantiator {
    template <ranges::forward_range Fwd>
    static constexpr void call() {
        const Fwd range{pairs};

        ASSERT(ranges::lower_bound(range, -1, ranges::less{}, get_first) == ranges::begin(range));
        ASSERT(ranges::lower_bound(ranges::begin(range), ranges::end(range), -1, ranges::less{}, get_first)
               == ranges::begin(range));

        for (int i = 0; i < 8; ++i) {
            const P* const target = pairs.data() + i + (i > 3 ? 2 : 0);
            ASSERT(ranges::lower_bound(range, i, ranges::less{}, get_first).peek() == target);
            ASSERT(ranges::lower_bound(ranges::begin(range), ranges::end(range), i, ranges::less{}, get_first).peek()
                   == target);
        }

        ASSERT(ranges::lower_bound(range, 8, ranges::less{}, get_first) == ranges::end(range));
        ASSERT(ranges::lower_bound(ranges::begin(range), ranges::end(range), 8, ranges::less{}, get_first)
               == ranges::end(range));
    }
};

struct upper_bound_instantiator {
    template <ranges::forward_range Fwd>
    static constexpr void call() {
        const Fwd range{pairs};

        ASSERT(ranges::upper_bound(range, -1, ranges::less{}, get_first) == ranges::begin(range));
        ASSERT(ranges::upper_bound(ranges::begin(range), ranges::end(range), -1, ranges::less{}, get_first)
               == ranges::begin(range));

        for (int i = 0; i < 8; ++i) {
            const P* const target = pairs.data() + i + 1 + (i >= 3 ? 2 : 0);

            ASSERT(ranges::upper_bound(range, i, ranges::less{}, get_first).peek() == target);
            ASSERT(ranges::upper_bound(ranges::begin(range), ranges::end(range), i, ranges::less{}, get_first).peek()
                   == target);
        }

        ASSERT(ranges::upper_bound(range, 8, ranges::less{}, get_first) == ranges::end(range));
        ASSERT(ranges::upper_bound(ranges::begin(range), ranges::end(range), 8, ranges::less{}, get_first)
               == ranges::end(range));
    }
};

struct equal_range_instantiator {
    template <ranges::forward_range Fwd>
    static constexpr void call() {
        const Fwd range{pairs};

        {
            auto result = ranges::equal_range(range, -1, ranges::less{}, get_first);
            ASSERT(result.begin() == ranges::begin(range));
            ASSERT(result.end() == ranges::begin(range));
        }
        {
            auto result = ranges::equal_range(ranges::begin(range), ranges::end(range), -1, ranges::less{}, get_first);
            ASSERT(result.begin() == ranges::begin(range));
            ASSERT(result.end() == ranges::begin(range));
        }

        for (int i = 0; i < 8; ++i) {
            const P* const lo = pairs.data() + i + (i > 3 ? 2 : 0);
            const P* const hi = pairs.data() + i + 1 + (i >= 3 ? 2 : 0);

            {
                auto result = ranges::equal_range(range, i, ranges::less{}, get_first);
                ASSERT(result.begin().peek() == lo);
                ASSERT(result.end().peek() == hi);
            }
            {
                auto result =
                    ranges::equal_range(ranges::begin(range), ranges::end(range), i, ranges::less{}, get_first);
                ASSERT(result.begin().peek() == lo);
                ASSERT(result.end().peek() == hi);
            }
        }

        {
            auto result = ranges::equal_range(range, 8, ranges::less{}, get_first);
            ASSERT(result.begin() == ranges::end(range));
            ASSERT(result.end() == ranges::end(range));
        }
        {
            auto result = ranges::equal_range(ranges::begin(range), ranges::end(range), 8, ranges::less{}, get_first);
            ASSERT(result.begin() == ranges::end(range));
            ASSERT(result.end() == ranges::end(range));
        }
    }
};

struct binary_search_instantiator {
    template <ranges::forward_range Fwd>
    static constexpr void call() {
        const Fwd range{pairs};

        ASSERT(!ranges::binary_search(range, -1, ranges::less{}, get_first));
        ASSERT(!ranges::binary_search(ranges::begin(range), ranges::end(range), -1, ranges::less{}, get_first));

        for (int i = 0; i < 8; ++i) {
            ASSERT(ranges::binary_search(range, i, ranges::less{}, get_first));
            ASSERT(ranges::binary_search(ranges::begin(range), ranges::end(range), i, ranges::less{}, get_first));
        }

        ASSERT(!ranges::binary_search(range, 8, ranges::less{}, get_first));
        ASSERT(!ranges::binary_search(ranges::begin(range), ranges::end(range), 8, ranges::less{}, get_first));
    }
};

int main() {
    STATIC_ASSERT((test_fwd<empty_ranges, const P>(), true));
    test_fwd<empty_ranges, const P>();

    STATIC_ASSERT((test_fwd<lower_bound_instantiator, const P>(), true));
    test_fwd<lower_bound_instantiator, const P>();

    STATIC_ASSERT((test_fwd<upper_bound_instantiator, const P>(), true));
    test_fwd<upper_bound_instantiator, const P>();

    STATIC_ASSERT((test_fwd<equal_range_instantiator, const P>(), true));
    test_fwd<equal_range_instantiator, const P>();

    STATIC_ASSERT((test_fwd<binary_search_instantiator, const P>(), true));
    test_fwd<binary_search_instantiator, const P>();
}
