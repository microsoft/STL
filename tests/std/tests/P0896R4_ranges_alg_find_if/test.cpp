// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <cstdlib>
#include <functional>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
//
#include <range_algorithm_support.hpp>

constexpr void smoke_test() {
    using ranges::find_if, ranges::iterator_t, std::same_as;
    using P = std::pair<int, int>;

    std::array<P, 3> const data = {{{0, 42}, {2, 42}, {4, 42}}};
    constexpr auto equals       = [](auto x) { return [x](auto&& y) { return y == x; }; };

    // Validate dangling story
    STATIC_ASSERT(same_as<decltype(find_if(borrowed<false>{}, equals(42))), ranges::dangling>);
    STATIC_ASSERT(same_as<decltype(find_if(borrowed<true>{}, equals(42))), int*>);

    for (auto [value, _] : data) {
        {
            // Validate range overload [found case]
            auto result = find_if(move_only_range{data}, equals(value), get_first);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<move_only_range<P const>>>);
            assert((*result).first == value);
        }
        {
            // Validate iterator+sentinel overload [found case]
            move_only_range x{data};
            auto result = find_if(x.begin(), x.end(), equals(value), get_first);
            STATIC_ASSERT(same_as<decltype(result), iterator_t<move_only_range<P const>>>);
            assert((*result).first == value);
        }
    }
    {
        // Validate range overload [not found case]
        auto result = find_if(move_only_range{data}, equals(42), get_first);
        STATIC_ASSERT(same_as<decltype(result), iterator_t<move_only_range<P const>>>);
        assert(result == move_only_range{data}.end());
    }
    {
        // Validate iterator+sentinel overload [not found case]
        move_only_range x{data};
        auto result = find_if(x.begin(), x.end(), equals(42), get_first);
        STATIC_ASSERT(same_as<decltype(result), iterator_t<move_only_range<P const>>>);
        assert(result == x.end());
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

        (void) ranges::find_if(in, UnaryPredicateFor<I>{});
        (void) ranges::find_if(in, ProjectedUnaryPredicate<>{}, ProjectionFor<I>{});
    }
};

template void test_in<instantiator>();
