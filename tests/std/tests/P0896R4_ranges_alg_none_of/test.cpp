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

constexpr auto is_even = [](auto const& x) { return x % 2 == 0; };
constexpr auto is_odd  = [](auto const& x) { return x % 2 != 0; };

constexpr void smoke_test() {
    using ranges::none_of;
    constexpr std::array<std::pair<int, int>, 3> data = {{{0, 13}, {7, 13}, {4, 13}}};

    assert(!none_of(move_only_range{data}, is_even, get_first));
    assert(none_of(move_only_range{data}, is_even, get_second));
    assert(!none_of(move_only_range{data}, is_odd, get_first));
    assert(!none_of(move_only_range{data}, is_odd, get_second));
    {
        move_only_range elements{data};
        assert(!none_of(elements.begin(), elements.end(), is_even, get_first));
    }
    {
        move_only_range elements{data};
        assert(none_of(elements.begin(), elements.end(), is_even, get_second));
    }
    {
        move_only_range elements{data};
        assert(!none_of(elements.begin(), elements.end(), is_odd, get_first));
    }
    {
        move_only_range elements{data};
        assert(!none_of(elements.begin(), elements.end(), is_odd, get_second));
    }
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In>
    static void call(In&& in = {}) {
        (void) ranges::none_of(in, UnaryPredicateFor<ranges::iterator_t<In>>{});
        (void) ranges::none_of(in, ProjectedUnaryPredicate<>{}, ProjectionFor<ranges::iterator_t<In>>{});
    }
};

template void test_in<instantiator>();
