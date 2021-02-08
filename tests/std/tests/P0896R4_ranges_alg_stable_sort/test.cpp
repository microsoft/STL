// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;
using P = pair<int, int>;

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::stable_sort(borrowed<false>{})), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::stable_sort(borrowed<true>{})), int*>);

struct instantiator {
    static constexpr array input = {P{1, 0}, P{-1260655766, 1}, P{-1298559576, 2}, P{1, 3}, P{-2095681771, 4},
        P{-441494788, 5}, P{-47163201, 6}, P{1, 7}, P{1429106719, 8}, P{1, 9}};

    template <ranges::random_access_range R>
    static void call() {
        using ranges::stable_sort, ranges::is_sorted, ranges::iterator_t, ranges::less;

        { // Validate range overload
            auto buff = input;
            const R range{buff};
            const same_as<iterator_t<R>> auto result = stable_sort(range, less{}, get_first);
            assert(result == range.end());
            assert(is_sorted(range)); // Check for stability by not using a projection
        }

        { // Validate iterator overload
            auto buff = input;
            const R range{buff};
            const same_as<iterator_t<R>> auto result = stable_sort(range.begin(), range.end(), less{}, get_first);
            assert(result == range.end());
            assert(is_sorted(range.begin(), range.end())); // Check for stability by not using a projection
        }

        { // Validate empty range
            const R range{};
            const same_as<iterator_t<R>> auto result = stable_sort(range, less{}, get_first);
            assert(result == range.end());
            assert(is_sorted(range, less{}));
        }
    }
};

int main() {
    test_random<instantiator, P>();
}
