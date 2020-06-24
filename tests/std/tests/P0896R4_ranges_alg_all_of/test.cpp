// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <array>
#include <cassert>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>

constexpr auto is_even = [](auto const& x) { return x % 2 == 0; };
constexpr auto is_odd  = [](auto const& x) { return x % 2 != 0; };

using R = std::array<std::pair<int, int>, 3>;

constexpr void smoke_test() {
    using ranges::all_of;
    constexpr R pairs = {{{0, 13}, {2, 13}, {4, 13}}};

    assert(all_of(move_only_range{pairs}, is_even, get_first));
    assert(!all_of(move_only_range{pairs}, is_even, get_second));
    assert(!all_of(move_only_range{pairs}, is_odd, get_first));
    assert(all_of(move_only_range{pairs}, is_odd, get_second));
    {
        move_only_range wrapped_pairs{pairs};
        assert(all_of(wrapped_pairs.begin(), wrapped_pairs.end(), is_even, get_first));
    }
    {
        move_only_range wrapped_pairs{pairs};
        assert(!all_of(wrapped_pairs.begin(), wrapped_pairs.end(), is_even, get_second));
    }
    {
        move_only_range wrapped_pairs{pairs};
        assert(!all_of(wrapped_pairs.begin(), wrapped_pairs.end(), is_odd, get_first));
    }
    {
        move_only_range wrapped_pairs{pairs};
        assert(all_of(wrapped_pairs.begin(), wrapped_pairs.end(), is_odd, get_second));
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In>
    static void call(In&& in = {}) {
        (void) ranges::all_of(in, UnaryPredicateFor<ranges::iterator_t<In>>{});
        (void) ranges::all_of(in, ProjectedUnaryPredicate<>{}, ProjectionFor<ranges::iterator_t<In>>{});
    }
};

template void test_in<instantiator>();
