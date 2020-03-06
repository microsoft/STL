// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>
//
#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::find_if_not, ranges::iterator_t, std::same_as;
    using P = std::pair<int, int>;

    std::array<P, 3> pairs = {{{0, 13}, {0, 13}, {0, 13}}};
    constexpr auto equals  = [](auto x) { return [x](auto&& y) { return y == x; }; };

    // Validate dangling story
    STATIC_ASSERT(same_as<decltype(find_if_not(borrowed<false>{}, equals(42))), ranges::dangling>);
    STATIC_ASSERT(same_as<decltype(find_if_not(borrowed<true>{}, equals(42))), int*>);

    for (auto& [value, _] : pairs) {
        value = 42;
        {
            // Validate range overload [found case]
            auto result = find_if_not(move_only_range{pairs}, equals(0), get_first);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<move_only_range<P>>>);
            assert((*result).first == 42);
        }
        {
            // Validate iterator + sentinel overload [found case]
            move_only_range wrapped_pairs{pairs};
            auto result = find_if_not(wrapped_pairs.begin(), wrapped_pairs.end(), equals(0), get_first);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<move_only_range<P>>>);
            assert((*result).first == 42);
        }
        value = 0;
    }
    {
        // Validate range overload [not found case]
        auto result = find_if_not(move_only_range{pairs}, equals(0), get_first);
        STATIC_ASSERT(same_as<decltype(result), iterator_t<move_only_range<P>>>);
        assert(result == move_only_range{pairs}.end());
    }
    {
        // Validate iterator + sentinel overload [not found case]
        move_only_range wrapped_pairs{pairs};
        auto result = find_if_not(wrapped_pairs.begin(), wrapped_pairs.end(), equals(0), get_first);
        STATIC_ASSERT(same_as<decltype(result), iterator_t<move_only_range<P>>>);
        assert(result == wrapped_pairs.end());
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In>
    static void call(In&& in = {}) {
        using ranges::iterator_t;
        using I = iterator_t<In>;

        (void) ranges::find_if_not(in, UnaryPredicateFor<I>{});
        (void) ranges::find_if_not(in, ProjectedUnaryPredicate<>{}, ProjectionFor<I>{});
    }
};

template void test_in<instantiator>();
