// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Covers ranges::is_partitioned and ranges::partition (and bits of ranges::partition_point)

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <numeric>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

#define ASSERT(...) assert((__VA_ARGS__))

using P = pair<int, int>;

constexpr auto is_even = [](int i) { return i % 2 == 0; };

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::partition(borrowed<false>{}, is_even)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::partition(borrowed<true>{}, is_even)), ranges::subrange<int*>>);

struct empty_test {
    template <ranges::input_range Range>
    static constexpr void call() {
        // Validate empty ranges
        using ranges::is_partitioned, ranges::partition, ranges::partition_point;
        {
            Range range{};
            ASSERT(is_partitioned(range, is_even, get_first));
        }
        {
            Range range{};
            ASSERT(is_partitioned(range.begin(), range.end(), is_even, get_first));
        }

        if constexpr (ranges::forward_range<Range>) {
            const Range range{};
            {
                const auto result = partition(range, is_even, get_first);
                ASSERT(result.begin() == range.end());
                ASSERT(result.end() == range.end());
            }
            {
                const auto result = partition(range.begin(), range.end(), is_even, get_first);
                ASSERT(result.begin() == range.end());
                ASSERT(result.end() == range.end());
            }
        }
    }
};

struct partition_test {
    static constexpr array elements = {P{0, 42}, P{1, 42}, P{2, 42}, P{3, 42}, P{4, 42}, P{5, 42}, P{6, 42}, P{7, 42}};

    template <ranges::input_range Range>
    static constexpr void call() {
        using ranges::is_partitioned, ranges::partition, ranges::partition_point, ranges::is_permutation,
            ranges::subrange;

        { // Validate is_partitioned
            auto pairs = elements;

            {
                Range range{pairs};
                ASSERT(!is_partitioned(range, is_even, get_first));
            }
            {
                Range range{pairs};
                ASSERT(!is_partitioned(range.begin(), range.end(), is_even, get_first));
            }
        }

        if constexpr (ranges::forward_range<Range>) {
            { // Validate range overloads of partition, is_partitioned, partition_point
                auto pairs = elements;
                const Range range{pairs};
                const auto mid = ranges::next(range.begin(), 4);

                {
                    auto result = partition(range, is_even, get_first);
                    ASSERT(result.begin() == mid);
                    ASSERT(result.end() == range.end());
                }
                ASSERT(is_permutation(subrange{range.begin(), mid}, array{0, 2, 4, 6}, ranges::equal_to{}, get_first));
                ASSERT(is_partitioned(range, is_even, get_first));
                ASSERT(partition_point(range, is_even, get_first) == mid);
            }

            { // Validate iterator overloads of partition, is_partitioned, partition_point
                auto pairs = elements;
                const Range range{pairs};
                const auto mid = ranges::next(range.begin(), 4);

                {
                    auto result = partition(range.begin(), range.end(), is_even, get_first);
                    ASSERT(result.begin() == mid);
                    ASSERT(result.end() == range.end());
                }
                ASSERT(is_permutation(subrange{range.begin(), mid}, array{0, 2, 4, 6}, ranges::equal_to{}, get_first));
                ASSERT(is_partitioned(range.begin(), range.end(), is_even, get_first));
                ASSERT(partition_point(range.begin(), range.end(), is_even, get_first) == mid);
            }
        }
    }
};

int main() {
    STATIC_ASSERT((test_in<empty_test, P>(), true));
    test_in<empty_test, P>();

    STATIC_ASSERT((test_in<partition_test, P>(), true));
    test_in<partition_test, P>();
}
