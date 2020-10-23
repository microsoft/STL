// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Covers ranges::push_heap, ranges::pop_heap, ranges::make_heap, ranges::is_heap, ranges::is_heap_until, and
// ranges::sort_heap

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
STATIC_ASSERT(same_as<decltype(ranges::push_heap(borrowed<false>{})), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::push_heap(borrowed<true>{})), int*>);
STATIC_ASSERT(same_as<decltype(ranges::pop_heap(borrowed<false>{})), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::pop_heap(borrowed<true>{})), int*>);
STATIC_ASSERT(same_as<decltype(ranges::make_heap(borrowed<false>{})), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::make_heap(borrowed<true>{})), int*>);
STATIC_ASSERT(same_as<decltype(ranges::sort_heap(borrowed<false>{})), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::sort_heap(borrowed<true>{})), int*>);
STATIC_ASSERT(same_as<decltype(ranges::is_heap_until(borrowed<false>{})), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::is_heap_until(borrowed<true>{})), int*>);

using P = pair<int, int>;

struct empty_ranges {
    template <ranges::random_access_range Range>
    static constexpr void call() {
        // Validate empty ranges (only make_heap and sort_heap accept empty ranges)
        const Range range{};

        ASSERT(ranges::make_heap(range, ranges::less{}, get_first) == ranges::end(range));
        ASSERT(ranges::make_heap(ranges::begin(range), ranges::end(range), ranges::less{}, get_first)
               == ranges::end(range));

        ASSERT(ranges::sort_heap(range, ranges::less{}, get_first) == ranges::end(range));
        ASSERT(ranges::sort_heap(ranges::begin(range), ranges::end(range), ranges::less{}, get_first)
               == ranges::end(range));
    }
};

struct is_heap_test {
    template <ranges::random_access_range Range>
    static constexpr void call() {
        // Validate is_heap and is_heap_until
        using ranges::is_heap, ranges::is_heap_until, ranges::less, ranges::swap;

        array buff = {
            P{1668617627, 0},
            P{1429106719, 1},
            P{-47163201, 2},
            P{-441494788, 3},
            P{-1200257975, 4},
            P{-1459960308, 5},
            P{-912489821, 6},
            P{-2095681771, 7},
            P{-1298559576, 8},
            P{-1260655766, 9},
        };
        const Range wrapped{buff};

        ASSERT(is_heap(wrapped, less{}, get_first));
        ASSERT(is_heap(wrapped.begin(), wrapped.end(), less{}, get_first));

        ASSERT(is_heap_until(wrapped, less{}, get_first) == wrapped.end());
        ASSERT(is_heap_until(wrapped.begin(), wrapped.end(), less{}, get_first) == wrapped.end());

        swap(buff[0], buff[1]);

        ASSERT(!is_heap(wrapped, less{}, get_first));
        ASSERT(!is_heap(wrapped.begin(), wrapped.end(), less{}, get_first));

        ASSERT(is_heap_until(wrapped, less{}, get_first) == wrapped.begin() + 1);
        ASSERT(is_heap_until(wrapped.begin(), wrapped.end(), less{}, get_first) == wrapped.begin() + 1);
    }
};

struct make_and_sort_heap_test {
    static constexpr array initial_values = {
        P{-1200257975, 0},
        P{-1260655766, 1},
        P{-1298559576, 2},
        P{-1459960308, 3},
        P{-2095681771, 4},
        P{-441494788, 5},
        P{-47163201, 6},
        P{-912489821, 7},
        P{1429106719, 8},
        P{1668617627, 9},
    };

    template <ranges::random_access_range Range>
    static constexpr void call() {
        // Validate make_heap and sort_heap
        using ranges::make_heap, ranges::sort_heap, ranges::is_heap, ranges::is_sorted, ranges::less, ranges::swap;

        {
            auto buff = initial_values;
            const Range wrapped{buff};

            ASSERT(!is_heap(wrapped, less{}, get_first));

            make_heap(wrapped, less{}, get_first);
            ASSERT(is_heap(wrapped, less{}, get_first));

            sort_heap(wrapped, less{}, get_first);
            ASSERT(is_sorted(wrapped, less{}, get_first));
        }

        {
            auto buff = initial_values;
            const Range wrapped{buff};

            ASSERT(!is_heap(wrapped.begin(), wrapped.end(), less{}, get_first));

            make_heap(wrapped.begin(), wrapped.end(), less{}, get_first);
            ASSERT(is_heap(wrapped.begin(), wrapped.end(), less{}, get_first));

            sort_heap(wrapped.begin(), wrapped.end(), less{}, get_first);
            ASSERT(is_sorted(wrapped.begin(), wrapped.end(), less{}, get_first));
        }
    }
};

struct push_and_pop_heap_test {
    static constexpr array initial_values = {
        P{1668617627, 0},
        P{1429106719, 1},
        P{-47163201, 2},
        P{-441494788, 3},
        P{-1200257975, 4},
        P{-1459960308, 5},
        P{-912489821, 6},
        P{-2095681771, 7},
        P{-1298559576, 8},
        P{-1260655766, 9},
    };
    STATIC_ASSERT(ranges::is_heap(initial_values, ranges::less{}, get_first));

    static constexpr array expectedPushed = {
        P{1668617627, 0},
        P{1429106719, 1},
        P{-47163201, 2},
        P{-1260655766, 9},
        P{-441494788, 3},
        P{-1459960308, 5},
        P{-912489821, 6},
        P{-2095681771, 7},
        P{-1298559576, 8},
        P{-1200257975, 4},
    };
    STATIC_ASSERT(ranges::is_heap(expectedPushed, ranges::less{}, get_first));

    static constexpr array expectedPopped = {
        P{1429106719, 1},
        P{-441494788, 3},
        P{-47163201, 2},
        P{-1260655766, 9},
        P{-1200257975, 4},
        P{-1459960308, 5},
        P{-912489821, 6},
        P{-2095681771, 7},
        P{-1298559576, 8},
        P{1668617627, 0},
    };
    STATIC_ASSERT(ranges::is_heap(expectedPopped.begin(), expectedPopped.end() - 1, ranges::less{}, get_first));

    template <ranges::random_access_range Range>
    static constexpr void call() {
        // Validate push_heap and pop_heap
        using ranges::push_heap, ranges::pop_heap, ranges::equal, ranges::is_heap, ranges::less;

        {
            auto buff = initial_values;
            const Range wrapped{buff};

            pop_heap(wrapped, less{}, get_first);
            ASSERT(equal(wrapped, expectedPopped));

            push_heap(wrapped, less{}, get_first);
            ASSERT(equal(wrapped, expectedPushed));
        }

        {
            auto buff = initial_values;
            const Range wrapped{buff};

            pop_heap(wrapped.begin(), wrapped.end(), less{}, get_first);
            ASSERT(is_heap(expectedPopped.begin(), expectedPopped.end() - 1, less{}, get_first));
            ASSERT(equal(wrapped.begin(), wrapped.end(), expectedPopped.begin(), expectedPopped.end()));

            push_heap(wrapped.begin(), wrapped.end(), less{}, get_first);
            ASSERT(is_heap(expectedPushed, less{}, get_first));
            ASSERT(equal(wrapped.begin(), wrapped.end(), expectedPushed.begin(), expectedPushed.end()));
        }
    }
};

int main() {
    STATIC_ASSERT((test_random<empty_ranges, P>(), true));
    test_random<empty_ranges, P>();

    STATIC_ASSERT((test_random<is_heap_test, P>(), true));
    test_random<is_heap_test, P>();

    STATIC_ASSERT((test_random<make_and_sort_heap_test, P>(), true));
    test_random<make_and_sort_heap_test, P>();

    STATIC_ASSERT((test_random<push_and_pop_heap_test, P>(), true));
    test_random<push_and_pop_heap_test, P>();
}
