// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

using namespace std;

// Validate dangling story
STATIC_ASSERT(same_as<decltype(ranges::nth_element(borrowed<false>{}, nullptr_to<int>)), ranges::dangling>);
STATIC_ASSERT(same_as<decltype(ranges::nth_element(borrowed<true>{}, nullptr_to<int>)), int*>);

using P = pair<int, int>;

struct instantiator {
    static constexpr int keys[] = {7, 6, 5, 4, 3, 2, 1, 0};

    template <ranges::random_access_range R>
    static constexpr void call() {
        using ranges::nth_element, ranges::all_of, ranges::find, ranges::iterator_t, ranges::less, ranges::none_of,
            ranges::size;

        P input[size(keys)];
        const auto init = [&] {
            for (size_t j = 0; j < size(keys); ++j) {
                input[j] = P{keys[j], static_cast<int>(10 + j)};
            }
        };

        // Validate range overload
        for (int i = 0; i < int{size(keys)}; ++i) {
            init();
            const R wrapped{input};
            const auto nth                           = wrapped.begin() + i;
            const same_as<iterator_t<R>> auto result = nth_element(wrapped, nth, less{}, get_first);
            assert(result == wrapped.end());
            assert((*nth == P{i, static_cast<int>(10 + (find(keys, i) - keys))}));
            if (nth != wrapped.end()) {
                assert(all_of(wrapped.begin(), nth, [&](auto&& x) { return get_first(x) <= get_first(*nth); }));
                assert(all_of(nth, wrapped.end(), [&](auto&& x) { return get_first(*nth) <= get_first(x); }));
            }
        }

        // Validate iterator overload
        for (int i = 0; i < int{size(keys)}; ++i) {
            init();
            const R wrapped{input};
            const auto nth = wrapped.begin() + i;
            const same_as<iterator_t<R>> auto result =
                nth_element(wrapped.begin(), nth, wrapped.end(), less{}, get_first);
            assert(result == wrapped.end());
            assert((input[i] == P{i, static_cast<int>(10 + (find(keys, i) - keys))}));
            if (nth != wrapped.end()) {
                assert(all_of(wrapped.begin(), nth, [&](auto&& x) { return get_first(x) <= get_first(*nth); }));
                assert(all_of(nth, wrapped.end(), [&](auto&& x) { return get_first(*nth) <= get_first(x); }));
            }
        }

        {
            // Validate empty range
            const R range{};
            const same_as<iterator_t<R>> auto result = nth_element(range, range.begin(), less{}, get_first);
            assert(result == range.end());
        }
    }
};

int main() {
    STATIC_ASSERT((test_random<instantiator, P>(), true));
    test_random<instantiator, P>();
}
