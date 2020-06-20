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
    using ranges::for_each_n, ranges::for_each_n_result, ranges::in_fun_result, ranges::iterator_t;
    using std::same_as;
    using P = std::pair<int, int>;
    using R = std::array<P, 3>;

    // Validate that for_each_n_result aliases in_fun_result
    STATIC_ASSERT(same_as<for_each_n_result<int, double>, in_fun_result<int, double>>);

    R pairs   = {{{0, 42}, {2, 42}, {4, 42}}};
    auto incr = [](auto& y) { ++y; };

    move_only_range wrapped_pairs{pairs};
    auto result = for_each_n(wrapped_pairs.begin(), ranges::distance(pairs), incr, get_first);
    STATIC_ASSERT(same_as<decltype(result), for_each_n_result<iterator_t<move_only_range<P>>, decltype(incr)>>);
    assert(result.in == wrapped_pairs.end());
    int some_value = 1729;
    result.fun(some_value);
    assert(some_value == 1730);
    R const expected = {{{1, 42}, {3, 42}, {5, 42}}};
    assert(ranges::equal(pairs, expected));
}

int main() {
    STATIC_ASSERT((smoke_test(), true));
    smoke_test();
}

struct instantiator {
    template <class In, class, class>
    static void call(In in = {}) {
        using std::iter_difference_t;

        using Fun = void (*)(std::iter_common_reference_t<In>);
        ranges::for_each_n(std::move(in), iter_difference_t<In>{}, Fun{});

        using ProjFun = void (*)(unique_tag<0>);
        ranges::for_each_n(std::move(in), iter_difference_t<In>{}, ProjFun{}, ProjectionFor<In>{});
    }
};

template void test_counted_write<instantiator>();
