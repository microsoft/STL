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

constexpr auto is_even = [](auto const& x) { return x % 2 == 0; };
constexpr auto is_odd  = [](auto const& x) { return x % 2 != 0; };

constexpr void smoke_test() {
    using ranges::count_if;
    using P                  = std::pair<int, int>;
    std::array<P, 5> const x = {{{0, 47}, {1, 99}, {2, 99}, {3, 47}, {4, 99}}};
    using D                  = ranges::range_difference_t<move_only_range<P const>>;

    {
        // Validate range overload
        auto result = count_if(move_only_range{x}, is_even, get_first);
        STATIC_ASSERT(std::same_as<decltype(result), D>);
        assert(result == 3);
    }
    {
        // Validate iterator + sentinel overload
        move_only_range wrapped_x{x};
        auto result = count_if(wrapped_x.begin(), wrapped_x.end(), is_odd, get_first);
        STATIC_ASSERT(std::same_as<decltype(result), D>);
        assert(result == 2);
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

        (void) ranges::count_if(in, UnaryPredicateFor<I>{});
        (void) ranges::count_if(in, ProjectedUnaryPredicate<>{}, ProjectionFor<I>{});
    }
};

template void test_in<instantiator>();
