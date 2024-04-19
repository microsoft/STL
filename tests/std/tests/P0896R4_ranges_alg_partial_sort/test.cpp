// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;
using P = pair<int, int>;

// Validate dangling story
static_assert(same_as<decltype(ranges::partial_sort(borrowed<false>{}, nullptr_to<int>)), ranges::dangling>);
static_assert(same_as<decltype(ranges::partial_sort(borrowed<true>{}, nullptr_to<int>)), int*>);

struct instantiator {
    static constexpr P sorted[] = {{0, 16}, {1, 12}, {2, 17}, {3, 13}, {4, 15}, {5, 11}, {6, 14}, {7, 10}};

    template <ranges::random_access_range R>
    static constexpr void call() {
        using ranges::partial_sort, ranges::equal, ranges::iterator_t, ranges::less, ranges::next, ranges::size;

        { // Validate range overload
            for (size_t i = 0; i <= size(sorted); ++i) {
                P elements[] = {{7, 10}, {5, 11}, {1, 12}, {3, 13}, {6, 14}, {4, 15}, {0, 16}, {2, 17}};
                const R range{elements};
                const auto middle                        = next(range.begin(), static_cast<int>(i));
                const same_as<iterator_t<R>> auto result = partial_sort(range, middle, less{}, get_first);
                assert(result == range.end());
                assert(equal(range.begin(), middle, sorted + 0, sorted + i));
            }
        }

        { // Validate iterator overload
            for (size_t i = 0; i <= size(sorted); ++i) {
                P elements[] = {{7, 10}, {5, 11}, {1, 12}, {3, 13}, {6, 14}, {4, 15}, {0, 16}, {2, 17}};
                const R range{elements};
                const auto middle = next(range.begin(), static_cast<int>(i));
                const same_as<iterator_t<R>> auto result =
                    partial_sort(range.begin(), middle, range.end(), less{}, get_first);
                assert(result == range.end());
                assert(equal(range.begin(), middle, sorted + 0, sorted + i));
            }
        }
    }
};

int main() {
    static_assert((test_random<instantiator, P>(), true));
    test_random<instantiator, P>();
}
