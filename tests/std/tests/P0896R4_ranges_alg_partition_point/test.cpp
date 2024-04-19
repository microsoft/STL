// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <numeric>
#include <ranges>
#include <span>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

#define ASSERT(...) assert((__VA_ARGS__))

using P = pair<int, int>;

constexpr auto is_even = [](int i) { return i % 2 == 0; };

// Validate dangling story
static_assert(same_as<decltype(ranges::partition_point(borrowed<false>{}, is_even)), ranges::dangling>);
static_assert(same_as<decltype(ranges::partition_point(borrowed<true>{}, is_even)), int*>);

struct empty_test {
    template <ranges::forward_range Range>
    static constexpr void call() {
        // Validate empty ranges
        using ranges::partition_point;

        const Range range{span<P, 0>{}};
        ASSERT(partition_point(range, is_even, get_first) == range.end());
        ASSERT(partition_point(range.begin(), range.end(), is_even, get_first) == range.end());
    }
};

struct partition_point_test {
    template <ranges::forward_range R>
    static constexpr void call() {
        using ranges::partition_point;
        int elements[200];
        iota(ranges::begin(elements), ranges::end(elements), 0);

        // to avoid constant expression step limits
        const size_t bound = elements[0] + (is_constant_evaluated() ? 10 : ranges::size(elements));

        for (size_t i = 0; i < bound; ++i) {
            const R range{span{elements}.first(i)};
            for (size_t j = 0; j < i; ++j) {
                assert(partition_point(range, [j](int x) { return x < static_cast<int>(j); }).peek() == elements + j);
            }
        }
    }
};

int main() {
    static_assert((test_fwd<empty_test, P>(), true));
    test_fwd<empty_test, P>();

    static_assert((test_fwd<partition_point_test, const int>(), true));
    test_fwd<partition_point_test, const int>();
}
